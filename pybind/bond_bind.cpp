#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(pyfi, m) {
    m.doc() = "pyfi"; // optional module docstring

    m.def("add", &add, "A function that adds two numbers");
}
