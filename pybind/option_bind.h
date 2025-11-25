//
// Created by Nikolay Tsonev on 25/11/2025.
//


#ifndef OPTION_BIND_H
#define OPTION_BIND_H

#include <pybind11/pybind11.h>

#include "../include/pyfi/option.h"

namespace py = pybind11;

void add_option_module(py::module_& m);

#endif // OPTION_BIND_H
