//
// Created by Nikolay Tsonev on 11/11/2025.
//

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "./bond_bind.h"

namespace py = pybind11;


PYBIND11_MODULE(pyfi, m) {
    m.doc() = "pyfi, a python financial instruments library. This library contains tools to help price options, find "
              "present values of bonds and standard brownian motion and GME";

    auto bond = m.def_submodule("bond", "Contains functions related to bond, their future, present values and more");
    add_bond_module(bond);
}
