#include "env.h"
#include "geom.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

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

py::array_t<float> convertObservation(std::vector<Observation> obs) {
  float *obsArray = new float[obs.size() * OBSERVATION_SIZE];
  for (unsigned int i=0; i < obs.size(); i++) {
    convertObservation(obs[i], i * OBSERVATION_SIZE, obsArray);
  }
  long unsigned int shape[] = {obs.size(), OBSERVATION_SIZE};
  return py::array_t<float>(shape, obsArray);
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
            [](Env &env, py::array_t<float> actionArr) {
              py::buffer_info actionArrInfo = actionArr.request();
              if (actionArrInfo.ndim != 2) {
                throw std::runtime_error("Number of dimensions must be 2");
              }
              if (actionArrInfo.shape[1] != 2) {
                throw std::runtime_error("Expected dimension[1] to be of size 2");
              }

              float* action = (float*)(actionArrInfo.ptr);
              int actionsCount = actionArrInfo.shape[0];
              Action actions[actionsCount];
              for (int i=0; i < actionsCount; i++) {
                actions[i] = Action{action[i * 2], action[i * 2 + 1]};
              }

              std::tuple<
                std::vector<Observation>,
                std::vector<float>,
                std::vector<char>
              > t = env.Step(actions);
              std::vector<Observation> obs = std::get<0>(t);
              std::vector<float> rewards = std::get<1>(t);
              std::vector<char> dones = std::get<2>(t);
              unsigned int tanksCount = 10;
              bool *donesArr = new bool[tanksCount]; // new float[dones.size()];
              for (unsigned int i=0; i < tanksCount; i++) {
                donesArr[i] = dones[i];
              }

              return std::make_tuple(
                  convertObservation(obs),
                  py::array_t<float>(rewards.size(), rewards.data()),
                  py::array_t<bool>(tanksCount, donesArr),
                  py::dict()
              );
            }
        );
}
