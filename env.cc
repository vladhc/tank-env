#include <tuple>
#include "point.h"
#include "tank.h"
#include "env.h"

Env::Env() :
  tank_(
      3, // max_speed
      0.3, // acceleration
      0.1, // angle_acceleration
      10.0, // fire_range
      20.0, // vision_range
      10, // max_fire_cooldown
      Point{0.0, 0.0}, // position
      0.0, // angle
      100 // hit_points
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
