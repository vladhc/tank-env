#include <iostream>
#include <tuple>
#include "env.h"

int main() {
  Env env;
  env.Reset();

  auto t = env.Step();
  Observation obs = std::get<0>(t);
  float reward = std::get<1>(t);
  bool done = std::get<2>(t);

  std::cout << "Reward: " << reward << std::endl;
  std::cout << "Done: " << done << std::endl;
  return 0;
}
