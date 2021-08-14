#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "env.h"

namespace py = pybind11;

py::array_t<double> convertObservation(Observation obs) {
    double *ret = new double[3];
    ret[0] = obs.tank.GetPosition().x;
    ret[1] = obs.tank.GetPosition().y;
    ret[2] = obs.tank.GetAngle();
    return py::array_t<double>(3, ret);
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
              auto t = env.Step(Action{true, Point{0., 0.}});
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
