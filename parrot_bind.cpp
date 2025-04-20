#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "parrot.h"  // Include the Parrot class header

namespace py = pybind11;

PYBIND11_MODULE(parrot, m) {
    py::class_<Parrot>(m, "Parrot")
        .def(py::init<std::string>())  // Bind the constructor
        .def("search", &Parrot::search, py::arg("target"), py::arg("case_flag") = false)
        .def("close", &Parrot::close)
        .def("matchA", &Parrot::matchA)
        .def("matchB", &Parrot::matchB)
        .def("print", &Parrot::print, py::arg("all_flag") = false, py::arg("verbose_flag") = false)
        .def("__bool__", &Parrot::operator bool);  // Bind the operator
}