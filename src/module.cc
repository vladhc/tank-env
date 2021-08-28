#include <iostream>
#include <map>
#include "env.h"
#include "geom.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

int writeBody(Tank* hero, b2Body* target, float* arr, unsigned int idx) {
  arr[idx++] = target->GetPosition().x;
  arr[idx++] = target->GetPosition().y;
  arr[idx++] = target->GetAngle();
  arr[idx++] = target->GetLinearVelocity().x;
  arr[idx++] = target->GetLinearVelocity().y;
  arr[idx++] = target->GetLinearVelocity().Length();

  // In hero coordinates
  b2Vec2 pos = hero->GetBody()->GetLocalPoint(target->GetPosition());
  arr[idx++] = pos.Length();
  arr[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);
  return idx;
}

// 10 + 8 (per 1 tank) x9 tanks
// const int OBSERVATION_SIZE = 82;
#define OBSERVATION_SIZE 82

void convertObservation(Observation obs, unsigned int idx, float* ret) {
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
      idx = writeBody(obs.hero, ally->GetBody(), ret, idx);
    }

    // StrategicPoint
    b2Vec2 pos = obs.hero->GetBody()->GetLocalPoint(obs.strategicPoint->GetPosition());
    ret[idx++] = pos.Length();
    ret[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);
}

py::dict convertObservation(std::vector<Observation> obs) {
  py::dict obsMap;
  for (unsigned int i=0; i < obs.size(); i++) {
    float *obsArray = new float[OBSERVATION_SIZE];
    convertObservation(obs[i], 0, obsArray);
    obsMap[py::int_(i)] = py::array_t<float>(OBSERVATION_SIZE, obsArray);
  }
  return obsMap;
}

PYBIND11_MODULE(tanks, m) {
    py::class_<Env>(m, "Env")
        .def(py::init<>())
        .def("reset",
            [](Env &env) {
              std::vector<Observation> obs = env.Reset();
              return convertObservation(obs);
            }
        )
        .def("step",
            [](Env &env, std::map<int, py::array_t<float>> actionsMap) {
              Action actions[actionsMap.size()];
              for (auto item : actionsMap) {
                py::buffer_info actionArrInfo = item.second.request();
                if (actionArrInfo.ndim != 1) {
                  throw std::runtime_error("Number of dimensions must be 1");
                }
                if (actionArrInfo.shape[0] != 3) {
                  throw std::runtime_error("Expected dimension[0] to be of size 3");
                }
                float* action = (float*)(actionArrInfo.ptr);
                actions[item.first] = Action{action[0], action[1], action[2] > 0.5};
              }

              std::tuple<
                std::vector<Observation>,
                std::vector<float>,
                std::vector<char>
              > t = env.Step(actions);
              std::vector<Observation> obs = std::get<0>(t);
              std::vector<float> rewards = std::get<1>(t);
              std::vector<char> dones = std::get<2>(t);

              py::dict rewardsMap;
              py::dict donesMap;
              for (unsigned int i=0; i < rewards.size(); i++) {
                py::int_ idx = py::int_(i);
                rewardsMap[idx] = rewards[i];
                donesMap[idx] = (bool)(dones[i]);
              }
              donesMap["__all__"] = env.EpisodeComplete();

              return std::make_tuple(
                  convertObservation(obs),
                  rewardsMap,
                  donesMap,
                  py::dict()
              );
            }
        );
}
