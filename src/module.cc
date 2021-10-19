#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <cmath>
#include "env.h"
#include "geom.h"
#include "renderer.h"

const int MAX_BULLETS_COUNT = 15; // 1 bullet per tank

// hero: 10 floats
// 1 tank: 10 floats
const int OBSERVATION_SIZE = 14 + 9 * 14 + MAX_BULLETS_COUNT * 3;

namespace py = pybind11;

int writeBullets(std::vector<Bullet*> bullets, float* arr, float arena_size, unsigned int idx) {
  int bulletsCount = 0;
  for (Bullet* bullet : bullets) {
    b2Body* body = bullet->GetBody();
    const auto x = body->GetPosition().x;
    if (x < -arena_size || x > arena_size) {
      continue;
    }
    const auto y = body->GetPosition().y;
    if (y < -arena_size || y > arena_size) {
      continue;
    }
    arr[idx++] = x;
    arr[idx++] = y;
    arr[idx++] = normalizeAngle(body->GetAngle());
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

int write(Tank* tank, Tank* hero, float* arr, unsigned int idx) {
  arr[idx++] = tank->GetTeamId() == hero->GetTeamId();
  arr[idx++] = tank->GetHitpoints();
  arr[idx++] = tank->GetFireCooldown();

  b2Body* body = tank->GetBody();
  b2Body* turret = tank->GetTurret();

  arr[idx++] = body->GetPosition().x;
  arr[idx++] = body->GetPosition().y;
  arr[idx++] = normalizeAngle(body->GetAngle());
  arr[idx++] = normalizeAngle(turret->GetAngle());
  arr[idx++] = body->GetLinearVelocity().x;
  arr[idx++] = body->GetLinearVelocity().y;
  arr[idx++] = body->GetLinearVelocity().Length();
  arr[idx++] = body->GetAngularVelocity();
  arr[idx++] = turret->GetAngularVelocity();

  // In hero coordinates
  b2Vec2 pos = hero->GetBody()->GetLocalPoint(body->GetPosition());
  arr[idx++] = pos.Length();
  arr[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);
  return idx;
}

py::array_t<float> createObservation(const Observation &obs) {
    float *ret = new float[OBSERVATION_SIZE];
    unsigned int idx = 0;
    // Arena
    ret[idx++] = obs.arenaSize;

    // Tank
    ret[idx++] = obs.hero->GetHitpoints();
    ret[idx++] = obs.hero->GetFireCooldown();

    const b2Body* body = obs.hero->GetBody();
    const b2Body* turret = obs.hero->GetTurret();
    ret[idx++] = body->GetPosition().x;
    ret[idx++] = body->GetPosition().y;
    ret[idx++] = normalizeAngle(body->GetAngle());
    ret[idx++] = normalizeAngle(turret->GetAngle());
    ret[idx++] = body->GetLinearVelocity().x;
    ret[idx++] = body->GetLinearVelocity().y;
    ret[idx++] = body->GetLinearVelocity().Length();
    ret[idx++] = body->GetAngularVelocity();
    ret[idx++] = turret->GetAngularVelocity();

    for (Tank* ally : obs.allies) {
      idx = write(ally, obs.hero, ret, idx);
    }

    // StrategicPoint
    b2Vec2 pos = body->GetLocalPoint(obs.strategicPoint->GetPosition());
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
                int tankId = obs[i].hero->GetId();
                py::int_ idx = py::int_(tankId);
                obsMap[idx] = createObservation(obs[i]);
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
                  int tankId = obs[i].hero->GetId();
                  py::int_ idx = py::int_(tankId);

                  obsMap[idx] = createObservation(obs[i]);
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
