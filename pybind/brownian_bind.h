//
// Created by Bilal Waraich on 10/01/2026.
//

#ifndef BROWNIAN_BIND_H
#define BROWNIAN_BIND_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void add_brownian_module(py::module_& m);

#endif // BROWNIAN_BIND_H