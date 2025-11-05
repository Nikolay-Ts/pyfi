//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

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
    const double pv_func = present_value<double>(flows, y, 0.0, years, m, /*same*/ false);
    REQUIRE(pv_func == Approx(pv_manual).margin(1e-12));
}

TEST_CASE("Redemption auto-added when flows shorter than tenor") {
    const double par = 1000.0, y = 0.05;
    const int years = 3, m = 1;

    const double pv = present_value<double>({}, y, par, years, m, /*same*/ false);
    REQUIRE(pv == Approx(1000.0 / std::pow(1.05, 3)).margin(1e-12)); // ~863.837598531476
}

TEST_CASE("Monotonicity: PV decreases as yield rises") {
    const double par = 1000.0;
    const int years = 10, m = 1;
    const double c = par * 0.08;
    const double pv_low = present_value<double>({c}, 0.07, par, years, m, true); // ~1070.2358
    const double pv_high = present_value<double>({c}, 0.09, par, years, m, true); // ~935.8234
    REQUIRE(pv_low > pv_high);
    REQUIRE(pv_low == Approx(1070.235815409326).margin(1e-9));
    REQUIRE(pv_high == Approx(935.8234229884099).margin(1e-9));
}

TEST_CASE("Invalid inputs throw") {
    REQUIRE_THROWS(present_value<double>({10.0}, 0.05, 1000.0, 5, 0, true));
    REQUIRE_THROWS(present_value<double>({10.0}, 0.05, 1000.0, -1, 2, true));
    REQUIRE_THROWS(present_value<double>({10.0}, -2.0, 1000.0, 5, 1, true));
}

TEST_CASE("IRR: par bond -> yield equals coupon", "[irr]") {
    using T = double;
    const int years = 5, m = 1;
    const T par = 1000;
    const T coupon = 0.06;

    const auto cfs = build_bond_cashflows(par, coupon, years, m);
    const T price = par; // at par when y == coupon

    const T irr = internal_rate_return<T>(cfs, price, /*interest_rate=*/coupon, par, years, m);

    REQUIRE(irr == Approx(coupon).epsilon(1e-12));
    REQUIRE(price_from_yield(cfs, irr, m) == Approx(price).epsilon(1e-12));
}

TEST_CASE("IRR: premium/discount bond", "[irr]") {
    using T = double;
    const int years = 7, m = 1;
    const T par = 1000;
    const T coupon = 0.06; // 6% coupon

    const auto cfs = build_bond_cashflows(par, coupon, years, m);

    {
        const T y_true = 0.07;
        const T price = price_from_yield(cfs, y_true, m);
        const T irr = internal_rate_return<T>(cfs, price, coupon, par, years, m);
        REQUIRE(irr == Approx(y_true).epsilon(1e-10));
    }

    {
        const T y_true = 0.05;
        const T price = price_from_yield(cfs, y_true, m);
        const T irr = internal_rate_return<T>(cfs, price, coupon, par, years, m);
        REQUIRE(irr == Approx(y_true).epsilon(1e-10));
    }
}

TEST_CASE("Forward value (continuous compounding): baseline check") {
    const double P0 = 950.0;
    const double r  = 0.05;
    const double T  = 2.0;

    const double expected = P0 * std::exp(r * T);
    const double got = forward_value(P0, r, T);

    REQUIRE(got == Approx(expected).epsilon(1e-12));
}

TEST_CASE("Forward value: zero rate and zero time edge cases") {
    const double P0 = 1234.56;

    // Zero rate -> no growth for any T
    for (double T : {0.0, 1.0, 5.0}) {
        REQUIRE(forward_value(P0, 0.0, T) == Approx(P0).epsilon(1e-12));
    }

    // Zero time -> immediate delivery, regardless of rate
    for (double r : {0.0, 0.01, 0.05, 0.20}) {
        REQUIRE(forward_value(P0, r, 0.0) == Approx(P0).epsilon(1e-12));
    }
}

TEST_CASE("Forward value: monotonicity in rate and time") {
    const double P0 = 1000.0;

    // Monotonic in rate for fixed T
    {
        const double T = 3.0;
        const double f_low  = forward_value(P0, 0.02, T);
        const double f_mid  = forward_value(P0, 0.05, T);
        const double f_high = forward_value(P0, 0.08, T);
        REQUIRE(f_low  < f_mid);
        REQUIRE(f_mid  < f_high);
    }

    // Monotonic in time for fixed r
    {
        const double r = 0.05;
        const double f_short = forward_value(P0, r, 1.0);
        const double f_long  = forward_value(P0, r, 5.0);
        REQUIRE(f_short < f_long);
    }
}

TEST_CASE("Forward value matches discrete compounding in the limit") {
    const double P0 = 777.77;
    const double r  = 0.073;  // 7.3% annual
    const double T  = 2.5;    // 2.5 years

    const double cont = forward_value(P0, r, T); // Continuous compounding: P0 * e^{rT}

    // Very high-frequency discrete compounding should approximate continuous
    const int m = 10000; // compounding periods per year
    // use exp and log1p for stability
    const double disc = P0 * std::exp((m * T) * std::log1p(r / m));

    // Relax epsilon slightly, because the error is O(1/m)
    REQUIRE(disc == Approx(cont).epsilon(1e-6));
}
