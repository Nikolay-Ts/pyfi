//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "pyfi/bond.h"

using namespace pyfi::bond;
using namespace Catch;


TEST_CASE("Par bond: coupon == yield -> price == par") {
    const double par = 1000.0, y = 0.05;
    for (int m : {1, 2, 4, 12}) {
        for (int Y : {1, 5, 30}) {
            // same_cashflows path: pass one period coupon value
            const double c = par * (y / m);
            const auto pv = present_value<double>({c}, y, par, Y, m, true);
            REQUIRE(pv == Approx(par).margin(1e-10));
        }
    }
}

TEST_CASE("Premium and discount pricing (semiannual)") {
    const double par = 1000.0;
    const int years = 5, m = 2;

    // 6% coupon, 5% yield -> ~1043.7603196548546
    {
        const double c = par * (0.06 / m);
        const double pv = present_value<double>({c}, 0.05, par, years, m, true);
        REQUIRE(pv == Approx(1043.7603196548546).margin(1e-9));
    }

    // 3% coupon, 5% yield -> ~912.479360690291
    {
        const double c = par * (0.03 / m);
        const double pv = present_value<double>({c}, 0.05, par, years, m, true);
        REQUIRE(pv == Approx(912.479360690291).margin(1e-9));
    }
}

TEST_CASE("Zero yield handled") {
    const double par = 1000.0;
    const int years = 3, m = 1;
    const double c = par * 0.04;
    const double pv = present_value<double>({c}, 0.0, par, years, m, true);
    REQUIRE(pv == Approx(1120.0).margin(1e-12));
}

TEST_CASE("General cash-flow path equals manual discounting") {
    const double y = 0.05;
    const int years = 3, m = 1;

    std::vector<double> flows{10.0, 10.0, 1010.0};
    const double pv_manual = 10.0 / 1.05 + 10.0 / std::pow(1.05, 2) + 1010.0 / std::pow(1.05, 3);
    const double pv_func = present_value<double>(flows, y, 0.0, years, m, /*same*/false);
    REQUIRE(pv_func == Approx(pv_manual).margin(1e-12));
}

TEST_CASE("Redemption auto-added when flows shorter than tenor") {
    const double par = 1000.0, y = 0.05;
    const int years = 3, m = 1;

    const double pv = present_value<double>({}, y, par, years, m, /*same*/false);
    REQUIRE(pv == Approx(1000.0 / std::pow(1.05, 3)).margin(1e-12));  // ~863.837598531476
}

TEST_CASE("Monotonicity: PV decreases as yield rises") {
    const double par = 1000.0;
    const int years = 10, m = 1;
    const double c = par * 0.08;
    const double pv_low  = present_value<double>({c}, 0.07, par, years, m, true); // ~1070.2358
    const double pv_high = present_value<double>({c}, 0.09, par, years, m, true); // ~935.8234
    REQUIRE(pv_low  > pv_high);
    REQUIRE(pv_low  == Approx(1070.235815409326).margin(1e-9));
    REQUIRE(pv_high == Approx(935.8234229884099).margin(1e-9));
}

TEST_CASE("Invalid inputs throw") {
    REQUIRE_THROWS(present_value<double>({10.0}, 0.05, 1000.0, 5, 0, true));
    REQUIRE_THROWS(present_value<double>({10.0}, 0.05, 1000.0, -1, 2, true));
    REQUIRE_THROWS(present_value<double>({10.0}, -2.0, 1000.0, 5, 1, true));
}