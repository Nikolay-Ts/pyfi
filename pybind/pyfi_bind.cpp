//
// Created by Nikolay Tsonev on 11/11/2025.
//

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "./bond_bind.cpp"
#include "./option_bind.cpp"

namespace py = pybind11;


PYBIND11_MODULE(_pyfi, m) {
    m.doc() = "pyfi, a python financial instruments library. This library contains tools to help price options, find "
              "present values of bonds and standard brownian motion and GME";

    auto bond = m.def_submodule("bond", "Contains functions related to bond, their future, present values and more");
    add_bond_module(bond);

    auto option = m.def_submodule("option",
        "Contains functions related to options pricing for american and european options alongside the greeks for the "
        "Black-Scholes formula");
    add_option_module(option);
}
