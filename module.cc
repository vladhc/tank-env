#include "env.h"
#include "geom.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

py::array_t<double> convertObservation(Observation obs) {
    double *ret = new double[9];
    int idx = 0;

    // Arena
    ret[idx++] = obs.arenaSize;

    // Tank
    ret[idx++] = obs.tank->GetPosition().x;
    ret[idx++] = obs.tank->GetPosition().y;
    ret[idx++] = normalizeAngle(obs.tank->GetAngle());
    ret[idx++] = obs.tank->GetBody()->GetLinearVelocity().x;
    ret[idx++] = obs.tank->GetBody()->GetLinearVelocity().y;
    ret[idx++] = obs.tank->GetBody()->GetAngularVelocity();

    // StrategicPoint
    ret[idx++] = obs.strategicPoint->GetPosition().x;
    ret[idx++] = obs.strategicPoint->GetPosition().y;

    return py::array_t<double>(9, ret);
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
            [](Env &env) {
              auto t = env.Step(Action{0.0f, 0.0f});
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
