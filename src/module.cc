#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cmath>
#include "env.h"
#include "geom.h"
#include "renderer.h"

const int MAX_BULLETS_COUNT = 4; // 1 bullet per tank

// hero: 10 floats
// 1 tank: 10 floats
const int OBSERVATION_SIZE = 14 + 1 * 15 + MAX_BULLETS_COUNT * 3;

namespace py = pybind11;

int writeBullets(const std::vector<const Bullet*> bullets, float* arr, float arena_size, unsigned int idx) {
  int bulletsCount = 0;
  for (auto bullet : bullets) {
    const auto x = bullet->GetPosition().x;
    if (x < -arena_size || x > arena_size) {
      continue;
    }
    const auto y = bullet->GetPosition().y;
    if (y < -arena_size || y > arena_size) {
      continue;
    }
    arr[idx++] = x;
    arr[idx++] = y;
    arr[idx++] = normalizeAngle(bullet->GetAngle());
    bulletsCount++;
  }
  while (bulletsCount < MAX_BULLETS_COUNT) {
    arr[idx++] = 0.;
    arr[idx++] = 0.;
    arr[idx++] = 0.;
    bulletsCount++;
  }
  return idx;
}

int write(const Tank* tank, const Tank* hero, float* arr, unsigned int idx) {
  arr[idx++] = tank->GetTeamId() == hero->GetTeamId();
  arr[idx++] = tank->GetHitpoints();
  arr[idx++] = tank->GetFireCooldown();

  arr[idx++] = tank->GetPosition().x - hero->GetPosition().x;
  arr[idx++] = tank->GetPosition().y - hero->GetPosition().y;
  arr[idx++] = normalizeAngle(tank->GetAngle());
  arr[idx++] = normalizeAngle(tank->GetTurretAngle());
  arr[idx++] = tank->GetLinearVelocity().x;
  arr[idx++] = tank->GetLinearVelocity().y;
  arr[idx++] = tank->GetLinearVelocity().Length();
  arr[idx++] = tank->GetAngularVelocity();
  arr[idx++] = tank->GetTurretAngularVelocity();

  // In hero coordinates
  b2Vec2 pos = hero->GetLocalPoint(tank->GetPosition());
  arr[idx++] = pos.Length();
  arr[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);
  b2Vec2 turretPos = hero->GetTurretLocalPoint(tank->GetPosition());
  arr[idx++] = normalizeAngle(atan2(turretPos.x, turretPos.y), true);
  return idx;
}

py::array_t<float> encodeObservation(const Observation &obs) {
    float *ret = new float[OBSERVATION_SIZE];
    unsigned int idx = 0;
    // Arena
    ret[idx++] = obs.arenaSize;

    // Tank
    const Tank* hero = obs.tanks[obs.heroId];
    ret[idx++] = hero->GetHitpoints();
    ret[idx++] = hero->GetFireCooldown();

    ret[idx++] = hero->GetPosition().x;
    ret[idx++] = hero->GetPosition().y;
    ret[idx++] = normalizeAngle(hero->GetAngle());
    ret[idx++] = normalizeAngle(hero->GetTurretAngle());
    ret[idx++] = hero->GetLinearVelocity().x;
    ret[idx++] = hero->GetLinearVelocity().y;
    ret[idx++] = hero->GetLinearVelocity().Length();
    ret[idx++] = hero->GetAngularVelocity();
    ret[idx++] = hero->GetTurretAngularVelocity();

    for (const Tank* tank : obs.tanks) {
      if (tank->GetId() == obs.heroId) {
        continue;
      }
      idx = write(tank, hero, ret, idx);
    }

    // StrategicPoint
    b2Vec2 pos = hero->GetLocalPoint(obs.strategicPoint->GetPosition());
    ret[idx++] = pos.Length();
    ret[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);

    idx = writeBullets(obs.bullets, ret, obs.arenaSize, idx);

    return py::array_t<float>(OBSERVATION_SIZE, ret);
}

PYBIND11_MODULE(tanks, m) {
    py::class_<Renderer>(m, "Renderer")
        .def(py::init<>())
        .def("render",
            [](Renderer &renderer, Env &env) {
              renderer.Render(env);
            });
    py::class_<Env>(m, "Env")
        .def(py::init<>())
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
                actions[item.first] = Action{action[0], action[1], action[2], action[3] > 0.5};
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
