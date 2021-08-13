#include <tuple>
#include "point.h"
#include "tank.h"
#include "env.h"

Env::Env() :
  tank_(
      Point{0.0, 0.0}, // position
      0.0 // angle
  )
{
}

Observation Env::Reset() {
  Observation obs{};
  return obs;
}

std::tuple<Observation, float, bool> Env::Step() {
  tank_.Tick();
  Observation obs{};
  float reward = 0.1;
  bool done = false;
  return std::make_tuple(obs, reward, done);
}
