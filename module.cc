#include "env.h"
#include "geom.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

py::array_t<double> convertObservation(Observation obs) {
    float *ret = new float[10];
    int idx = 0;

    // Arena
    ret[idx++] = obs.arenaSize;

    // Tank
    ret[idx++] = obs.tank->GetPosition().x;
    ret[idx++] = obs.tank->GetPosition().y;
    ret[idx++] = normalizeAngle(obs.tank->GetAngle());
    ret[idx++] = obs.tank->GetBody()->GetLinearVelocity().x;
    ret[idx++] = obs.tank->GetBody()->GetLinearVelocity().y;
    ret[idx++] = obs.tank->GetBody()->GetLinearVelocity().Length();
    ret[idx++] = obs.tank->GetBody()->GetAngularVelocity();

    // StrategicPoint
    b2Vec2 pos = obs.tank->GetBody()->GetLocalPoint(obs.strategicPoint->GetPosition());
    ret[idx++] = pos.Length();
    ret[idx++] = normalizeAngle(atan2(pos.x, pos.y), true);

    return py::array_t<float>(10, ret);
}

PYBIND11_MODULE(tanks, m) {
    py::class_<Env>(m, "Env")
        .def(py::init<>())
        .def("reset",
            [](Env &env) {
              Observation obs = env.Reset();
              return convertObservation(obs);
            }
        )
        .def("step",
            [](Env &env, py::array_t<float> actionArr) {
              py::buffer_info actionArrInfo = actionArr.request();
              if (actionArrInfo.ndim != 1) {
                throw std::runtime_error("Number of dimensions must be 1");
              }
              if (actionArrInfo.shape[0] != 2) {
                throw std::runtime_error("Expected dimension[0] to be of size 2");
              }

              float* action = (float*)(actionArrInfo.ptr);

              auto t = env.Step(Action{action[0], action[1]});
              Observation obs = std::get<0>(t);
              double reward = std::get<1>(t);
              bool done = std::get<2>(t);

              return std::make_tuple(
                  convertObservation(obs),
                  reward,
                  done,
                  py::dict()
              );
            }
        );
}
