//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "pyfi/Bond.h"


TEST_CASE("Test vector length") {
    constexpr auto start = 500;
    constexpr auto stop  = 1200;
    constexpr auto scale = 120.0F;
    constexpr size_t n = stop - start;

    std::vector<float> c(n);
    for (size_t i = 0; i < n; ++i)g
        c[i] = scale * (start + static_cast<float>(i));
    auto myBond = pyfi::Bond<float>(c, 5.0, 1, 1);

    REQUIRE(myBond.cashflows().size() == n);
}