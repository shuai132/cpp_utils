//#define ENABLE_TEST
#ifdef ENABLE_TEST
#include "Settings.hpp"

int main() {
  Settings setting;

  printf("name: %s\n", setting.get_value("name").c_str());
  printf("name: %s\n", setting.get_value("age").c_str());
  printf("name: %s\n", setting.get_value("age2").c_str());

  setting.set_value("name", "xiao_ming");
  setting.set_value("age", "18");
  setting.set_value("age2", "18");
  setting.rm_value("age2");

  return 0;
}
#else
int main() {
  return 0;
}
#endif
