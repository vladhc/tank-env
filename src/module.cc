#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cmath>
#include "env.h"
#include "geom.h"
#include "renderer.h"
#include "chunk.h"

const unsigned int FLOATS_PER_BULLET = 4;
const unsigned int BULLETS_PER_TANK = 0; // 2

namespace py = pybind11;

int writeBullets(
    const std::vector<const Bullet*> bullets,
    const Tank* hero,
    float* arr,
    unsigned int idx,
    unsigned int maxBulletsCount
) {
  unsigned int bulletsCount = 0;
  for (auto bullet : bullets) {
    const b2Vec2 pos = bullet->GetPosition();
    const b2Vec2 heroPos = hero->GetPosition();
    arr[idx++] = pos.x - heroPos.x;
    arr[idx++] = pos.y - heroPos.y;

    const b2Vec2 velocity = bullet->GetLinearVelocity();
    arr[idx++] = velocity.x;
    arr[idx++] = velocity.y;
    bulletsCount++;
  }
  while (bulletsCount < maxBulletsCount) {
    for (unsigned int i=0; i < FLOATS_PER_BULLET; i++) {
      arr[idx++] = 0.;
    }
    bulletsCount++;
  }
  return idx;
}

py::array_t<float> encodeObservation(const Observation &obs) {
    const auto tanksCount = obs.tanks.size();
    const auto maxBulletsCount = tanksCount * BULLETS_PER_TANK;
    const unsigned int observationSize = static_cast<unsigned int>(HeroChunk::Size) +
      (tanksCount - 1) * static_cast<unsigned int>(TankChunk::Size) +
      maxBulletsCount * FLOATS_PER_BULLET;

    float *ret = new float[observationSize];
    unsigned int idx = 0;

    // Hero
    const Tank* hero = obs.tanks[obs.heroId];
    writeHeroChunk(hero, ret + idx);
    idx += static_cast<unsigned int>(HeroChunk::Size);

    // Other tanks
    for (const Tank* tank : obs.tanks) {
      if (tank->GetId() == obs.heroId) {
        continue;
      }
      writeTankChunk(tank, hero, ret + idx);
      idx += static_cast<unsigned int>(TankChunk::Size);
    }

    // StrategicPoint
    // b2Vec2 pos = hero->GetLocalPoint(obs.strategicPoint->GetPosition());
    // ret[idx++] = pos.Length();
    // ret[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);

    // idx = writeBullets(obs.bullets, hero, ret, idx, maxBulletsCount);

    return py::array_t<float>(observationSize, ret);
}

PYBIND11_MODULE(tanks, m) {
    py::class_<Renderer>(m, "Renderer")
        .def(py::init<>())
        .def("render",
            [](Renderer &renderer, Env &env) {
              renderer.Render(env);
            });
    py::enum_<TankChunk>(m, "TankChunk")
      .value("IS_ENEMY", TankChunk::IS_ENEMY)
      .value("HITPOINTS", TankChunk::HITPOINTS)
      .value("FIRE_COOLDOWN", TankChunk::FIRE_COOLDOWN)
      .value("POSITION_DISTANCE", TankChunk::POSITION_DISTANCE)
      .value("POSITION_ANGLE", TankChunk::POSITION_ANGLE)
      .value("BODY_ANGLE", TankChunk::BODY_ANGLE)
      .value("TURRET_ANGLE_RELATIVE_TO_BODY", TankChunk::TURRET_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_LENGTH", TankChunk::VELOCITY_LENGTH)
      .value("VELOCITY_ANGLE_RELATIVE_TO_BODY", TankChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)
      .value("BODY_ANGULAR_VELOCITY", TankChunk::BODY_ANGULAR_VELOCITY)
      .value("TURRET_ANGULAR_VELOCITY", TankChunk::TURRET_ANGULAR_VELOCITY)
      .value("AIM_ANGLE", TankChunk::AIM_ANGLE)
      .value("Size", TankChunk::Size);
    py::enum_<HeroChunk>(m, "HeroChunk")
      .value("HITPOINTS", HeroChunk::HITPOINTS)
      .value("FIRE_COOLDOWN", HeroChunk::FIRE_COOLDOWN)
      .value("POSITION_X", HeroChunk::POSITION_X)
      .value("POSITION_Y", HeroChunk::POSITION_Y)
      .value("BODY_ANGLE", HeroChunk::BODY_ANGLE)
      .value("TURRET_ANGLE_RELATIVE_TO_BODY", HeroChunk::TURRET_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_ANGLE_RELATIVE_TO_BODY", HeroChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)
      .value("VELOCITY_LENGTH", HeroChunk::VELOCITY_LENGTH)
      .value("BODY_ANGULAR_VELOCITY", HeroChunk::BODY_ANGULAR_VELOCITY)
      .value("TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY", HeroChunk::TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY)
      .value("Size", HeroChunk::Size);
    py::class_<Env>(m, "Env")
        .def(py::init<unsigned int>())
        .def("damage_tank",
            [](Env &env, int tankId, unsigned int damage) {
              env.DamageTank(tankId, damage);
            }
        )
        .def("reset",
            [](Env &env) {
              std::vector<Observation> obs = env.Reset();

              py::dict obsMap;
              for (unsigned int i=0; i < obs.size(); i++) {
                const int tankId = obs[i].heroId;
                py::int_ idx = py::int_(tankId);
                obsMap[idx] = encodeObservation(obs[i]);
              }

              return obsMap;
            }
        )
        .def("step",
            [](Env &env, std::map<int, py::array_t<float>> actionsMap) {
              std::map<int, Action> actions;
              for (auto item : actionsMap) {
                py::buffer_info actionArrInfo = item.second.request();
                if (actionArrInfo.ndim != 1) {
                  throw std::runtime_error("Number of dimensions must be 1");
                }
                if (actionArrInfo.shape[0] != 4) {
                  throw std::runtime_error("Expected dimension[0] to be of size 4");
                }
                float* action = (float*)(actionArrInfo.ptr);
                actions[item.first] = Action{action[0], action[1], action[2], action[3] > 0.};
              }

              std::tuple<
                std::vector<Observation>,
                std::vector<float>,
                std::vector<char>
              > t = env.Step(actions);
              std::vector<Observation> obs = std::get<0>(t);
              std::vector<float> rewards = std::get<1>(t);
              std::vector<char> dones = std::get<2>(t);

              py::dict obsMap;
              py::dict rewardsMap;
              py::dict donesMap;

              for (unsigned int i=0; i < obs.size(); i++) {
                bool done = (bool)dones[i];
                float reward = rewards[i];

                if (!done || reward != 0) {
                  const int tankId = obs[i].heroId;
                  py::int_ idx = py::int_(tankId);

                  obsMap[idx] = encodeObservation(obs[i]);
                  rewardsMap[idx] = reward;
                  donesMap[idx] = done;
                }
              }
              donesMap["__all__"] = env.EpisodeComplete();

              return std::make_tuple(
                  obsMap,
                  rewardsMap,
                  donesMap,
                  py::dict()
              );
            }
        );
}
