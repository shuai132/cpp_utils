#include <algorithm>

class PID {
 public:
  PID(double p, double i, double d, double mi, double mo)
      : kp(p), ki(i), kd(d), maxInt(mi), maxOut(mo), error(0), lastError(0), integral(0), output(0) {}

  static double limit(double value, double min, double max) {
    return (value < min) ? min : (value > max ? max : value);
  }

  void calc(double ref, double fdb) {
    lastError = error;
    error = ref - fdb;
    double pErr = error * kp;
    double dErr = (error - lastError) * kd;
    integral += ki * error;
    integral = limit(integral, -maxInt, maxInt);
    double sumErr = pErr + dErr + integral;
    output = limit(sumErr, -maxOut, maxOut);
  }

  void clear() {
    error = lastError = integral = output = 0;
  }

  double getOutput() const {
    return output;
  }

 private:
  double kp, ki, kd;
  double maxInt, maxOut;
  double error, lastError, integral, output;
};

class CascadePID {
 public:
  CascadePID(PID pid_inner, PID pid_outer) : inner(pid_inner), outer(pid_outer), output(0) {}

  void calc(double outRef, double outFdb, double inFdb) {
    outer.calc(outRef, outFdb);
    inner.calc(outer.getOutput(), inFdb);
    output = inner.getOutput();
  }

  void clear() {
    inner.clear();
    outer.clear();
  }

  double getOutput() const {
    return output;
  }

 private:
  PID inner;
  PID outer;
  double output;
};
