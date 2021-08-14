#include <pybind11/pybind11.h>
#include "env.h"

namespace py = pybind11;

PYBIND11_MODULE(tanks, m) {
    py::class_<Env>(m, "Env")
        .def(py::init<>())
        .def("reset", &Env::Reset)
        .def("step", &Env::Step);
}
