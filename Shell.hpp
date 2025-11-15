#pragma once

#ifdef _WIN32
#include <io.h>
#include <process.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif
#else
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#endif

#include <array>
#include <cstring>
#include <mutex>
#include <ostream>
#include <string>

#include "defer.hpp"

namespace Shell {

// clang-format off
enum ExitCode {
  SUCCESS = 0,
  POPEN_FAILED = -1,
  PCLOSE_FAILED = -2,
  READ_ERROR = -3,
  EXCEPTION_ERROR = -4,
  STDIN_SAVE_ERROR = -5,
  OTHER_ERROR = -6
};
// clang-format on

struct Result {
  int exitCode;
  std::string output;
  bool operator==(const Result &rhs) const {
    return output == rhs.output && exitCode == rhs.exitCode;
  }
  bool operator!=(const Result &rhs) const {
    return !(rhs == *this);
  }
  explicit operator bool() const {
    return exitCode == 0;
  }
  friend std::ostream &operator<<(std::ostream &os, const Result &result) {
    os << "exit code: " << result.exitCode << " output: " << result.output;
    return os;
  }
};

Result exec(const std::string &command) {
  static std::mutex mutex;

  std::string result;

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
  // On Windows, we don't need to handle SIGCHLD
#else
  struct sigaction act_tmp, act_old;  // NOLINT
  act_tmp.sa_handler = SIG_DFL;
  sigemptyset(&act_tmp.sa_mask);
  act_tmp.sa_flags = 0;

  sigaction(SIGCHLD, &act_tmp, &act_old);
  defer {
    sigaction(SIGCHLD, &act_old, nullptr);
  };
#endif

#ifndef _WIN32
  // Save original stdin and redirect to /dev/null to avoid deadlock
  int stdin_save = dup(STDIN_FILENO);
  if (stdin_save == -1) {
#ifdef _WIN32
    char error_msg[256];
    strerror_s(error_msg, sizeof(error_msg), errno);
    return {static_cast<int>(ExitCode::STDIN_SAVE_ERROR), "Failed to save stdin: " + std::string(error_msg)};
#else
    return {static_cast<int>(ExitCode::STDIN_SAVE_ERROR), "Failed to save stdin: " + std::string(strerror(errno))};
#endif
  }
  defer {
    dup2(stdin_save, STDIN_FILENO);
    close(stdin_save);
  };

  // avoid deadlock by redirecting stdin
  int fp = open("/dev/null", O_RDONLY);  // Changed to O_RDONLY for stdin
  if (fp >= 0) {
    dup2(fp, STDIN_FILENO);
    close(fp);
  }
#endif

  mutex.lock();
  FILE *pipe = popen(command.c_str(), "r");
  mutex.unlock();

  if (pipe == nullptr) {
    return {static_cast<int>(ExitCode::POPEN_FAILED), "popen() failed!"};
  }

  // Note: fread() does not throw C++ exceptions, so try-catch is not useful here
  // It only returns the number of items read, and sets error flags on the FILE*
  std::array<char, 1024> buffer{};
  std::size_t bytesRead;
  while ((bytesRead = std::fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
    if (ferror(pipe)) {
      // Check for error during read
      pclose(pipe);
#ifdef _WIN32
      char error_msg[256];
      strerror_s(error_msg, sizeof(error_msg), errno);
      return {static_cast<int>(ExitCode::READ_ERROR), "Error reading from command: " + std::string(error_msg)};
#else
      return {static_cast<int>(ExitCode::READ_ERROR), "Error reading from command: " + std::string(strerror(errno))};
#endif
    }
    result += std::string(buffer.data(), bytesRead);
  }

  int ret = pclose(pipe);
  if (ret == -1) {
#ifdef _WIN32
    char error_msg[256];
    strerror_s(error_msg, sizeof(error_msg), errno);
    return Result{static_cast<int>(ExitCode::PCLOSE_FAILED), "pclose() failed: " + std::string(error_msg)};
#else
    return Result{static_cast<int>(ExitCode::PCLOSE_FAILED), "pclose() failed: " + std::string(strerror(errno))};
#endif
  } else {
#ifndef _WIN32
    if (WIFEXITED(ret)) {  // normal exit()
      int exitcode = WEXITSTATUS(ret);
      return Result{exitcode, result};
    } else {  // other error (e.g., killed by signal)
      return Result{static_cast<int>(ExitCode::OTHER_ERROR), result};
    }
#else
    // On Windows, popen/pclose doesn't give detailed exit information
    return Result{ret, result};
#endif
  }
}

}  // namespace Shell
