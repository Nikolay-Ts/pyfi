//
// Created by Nikolay Tsonev on 25/11/2025.
//

#ifndef BOND_BIND_H
#define BOND_BIND_H

#include <pybind11/pybind11.h>

namespace py = pybind11;

void add_bond_module(py::module_& m);

#endif // BOND_BIND_H
