#include <iostream>
#include <map>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "env.h"
#include "geom.h"
#include "renderer.h"

namespace py = pybind11;

int write(Tank* tank, Tank* hero, float* arr, unsigned int idx) {
  arr[idx++] = tank->GetTeamId() == hero->GetTeamId();

  b2Body* target = tank->GetBody();
  b2Body* turret = tank->GetTurret();

  arr[idx++] = target->GetPosition().x;
  arr[idx++] = target->GetPosition().y;
  arr[idx++] = target->GetAngle();
  arr[idx++] = turret->GetAngle();
  arr[idx++] = target->GetLinearVelocity().x;
  arr[idx++] = target->GetLinearVelocity().y;
  arr[idx++] = target->GetLinearVelocity().Length();

  // In hero coordinates
  b2Vec2 pos = hero->GetBody()->GetLocalPoint(target->GetPosition());
  arr[idx++] = pos.Length();
  arr[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);
  return idx;
}

// hero: 11 floats
// 1 tank: 10 floats
const int OBSERVATION_SIZE = 11 + 9 * 10;

py::array_t<float> createObservation(const Observation &obs) {
    float *ret = new float[OBSERVATION_SIZE];
    unsigned int idx = 0;
    // Arena
    ret[idx++] = obs.arenaSize;

    // Tank
    ret[idx++] = obs.hero->GetPosition().x;
    ret[idx++] = obs.hero->GetPosition().y;
    ret[idx++] = normalizeAngle(obs.hero->GetAngle());
    ret[idx++] = obs.hero->GetBody()->GetLinearVelocity().x;
    ret[idx++] = obs.hero->GetBody()->GetLinearVelocity().y;
    ret[idx++] = obs.hero->GetBody()->GetLinearVelocity().Length();
    ret[idx++] = obs.hero->GetBody()->GetAngularVelocity();

    for (Tank* ally : obs.allies) {
      idx = write(ally, obs.hero, ret, idx);
    }

    // StrategicPoint
    b2Vec2 pos = obs.hero->GetBody()->GetLocalPoint(obs.strategicPoint->GetPosition());
    ret[idx++] = pos.Length();
    ret[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);

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
