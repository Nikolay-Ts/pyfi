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
            const auto pv = present_value({c}, y, par, Y, m, true);
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
        const double pv = present_value({c}, 0.05, par, years, m, true);
        REQUIRE(pv == Approx(1043.7603196548546).margin(1e-9));
    }

    // 3% coupon, 5% yield -> ~912.479360690291
    {
        const double c = par * (0.03 / m);
        const double pv = present_value({c}, 0.05, par, years, m, true);
        REQUIRE(pv == Approx(912.479360690291).margin(1e-9));
    }
}

TEST_CASE("Zero yield handled") {
    const double par = 1000.0;
    const int years = 3, m = 1;
    const double c = par * 0.04;
    const double pv = present_value({c}, 0.0, par, years, m, true);
    REQUIRE(pv == Approx(1120.0).margin(1e-12));
}

TEST_CASE("General cash-flow path equals manual discounting") {
    const double y = 0.05;
    const int years = 3, m = 1;

    std::vector<double> flows{10.0, 10.0, 1010.0};
    const double pv_manual = 10.0 / 1.05 + 10.0 / std::pow(1.05, 2) + 1010.0 / std::pow(1.05, 3);
    const double pv_func = present_value(flows, y, 0.0, years, m, /*same*/ false);
    REQUIRE(pv_func == Approx(pv_manual).margin(1e-12));
}

TEST_CASE("Redemption auto-added when flows shorter than tenor") {
    const double par = 1000.0, y = 0.05;
    const int years = 3, m = 1;

    const double pv = present_value({}, y, par, years, m, /*same*/ false);
    REQUIRE(pv == Approx(1000.0 / std::pow(1.05, 3)).margin(1e-12)); // ~863.837598531476
}

TEST_CASE("Monotonicity: PV decreases as yield rises") {
    const double par = 1000.0;
    const int years = 10, m = 1;
    const double c = par * 0.08;
    const double pv_low = present_value({c}, 0.07, par, years, m, true); // ~1070.2358
    const double pv_high = present_value({c}, 0.09, par, years, m, true); // ~935.8234
    REQUIRE(pv_low > pv_high);
    REQUIRE(pv_low == Approx(1070.235815409326).margin(1e-9));
    REQUIRE(pv_high == Approx(935.8234229884099).margin(1e-9));
}

TEST_CASE("Invalid inputs throw") {
    REQUIRE_THROWS(present_value({10.0}, 0.05, 1000.0, 5, 0,  true));  // m <= 0
    REQUIRE_THROWS(present_value({10.0}, 0.05, 1000.0, -1, 2, true));  // years < 0
    REQUIRE_THROWS(present_value({10.0}, -2.0, 1000.0, 5, 1, true));   // rate <= -100%/period
}

TEST_CASE("IRR: par bond -> yield equals coupon", "[irr]") {
    using T = double;
    const int years = 5, m = 1;
    const T par = 1000;
    const T coupon = 0.06;

    const auto cfs = build_bond_cashflows(par, coupon, years, m);
    const T price = par; // at par when y == coupon

    const T irr = internal_rate_return(cfs, price, /*interest_rate=*/coupon, par, years, m);

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
        const T irr = internal_rate_return(cfs, price, coupon, par, years, m);
        REQUIRE(irr == Approx(y_true).epsilon(1e-10));
    }

    {
        const T y_true = 0.05;
        const T price = price_from_yield(cfs, y_true, m);
        const T irr = internal_rate_return(cfs, price, coupon, par, years, m);
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

TEST_CASE("Forward value: zero edge cases") {
    const double P0 = 1234.56;
    for (double T : {0.0, 1.0, 5.0})
        REQUIRE(forward_value(P0, 0.0, T) == Approx(P0).epsilon(1e-12));
    for (double r : {0.0, 0.01, 0.05, 0.20})
        REQUIRE(forward_value(P0, r, 0.0) == Approx(P0).epsilon(1e-12));
}

TEST_CASE("Forward value: monotonicity in rate and time") {
    const double P0 = 1000.0;

    {
        const double T = 3.0;
        const double f_low  = forward_value(P0, 0.02, T);
        const double f_mid  = forward_value(P0, 0.05, T);
        const double f_high = forward_value(P0, 0.08, T);
        REQUIRE(f_low  < f_mid);
        REQUIRE(f_mid  < f_high);
    }

    {
        const double r = 0.05;
        const double f_short = forward_value(P0, r, 1.0);
        const double f_long  = forward_value(P0, r, 5.0);
        REQUIRE(f_short < f_long);
    }
}

TEST_CASE("Forward value matches discrete compounding in the limit") {
    const double P0 = 777.77;
    const double r  = 0.073; 
    const double T  = 2.5;    

    const double cont = forward_value(P0, r, T); 
    const int m = 10000; 
    const double disc = P0 * std::exp((m * T) * std::log1p(r / m));
    REQUIRE(disc == Approx(cont).epsilon(1e-6));
}

TEST_CASE("Zero-coupon (discrete) matches textbook", "[bond][zero]") {
    const double F = 1000, y = 0.06, T = 5; const int m = 2;
    REQUIRE(zero_coupon_price(F, y, T, m)
            == Approx(F / std::pow(1.0 + y/m, m*T)).epsilon(1e-12));
}

TEST_CASE("Coupon bond: par when coupon==yield", "[bond][coupon]") {
    const double F = 1000, c = 0.05, y = 0.05, T = 10; const int m = 2;
    REQUIRE(coupon_bond_price(F, c, y, T, m) == Approx(F).epsilon(1e-12));
}

TEST_CASE("Coupon bond: premium/discount", "[bond][coupon]") {
    const double F = 1000, T = 7; const int m = 2;
    const double prem = coupon_bond_price(F, 0.07, 0.05, T, m);
    const double disc = coupon_bond_price(F, 0.03, 0.05, T, m);
    REQUIRE(prem > F);
    REQUIRE(disc < F);
}

TEST_CASE("Clean = Dirty - Accrued; wrappers from_T match explicit") {
    const double P = 1000.0, c = 0.06, y = 0.05; const int m = 2;

    // Example: T = 3.75y → N = 7.5 → n = 8, alpha = 0.5
    const double T = 3.75;
    const double N = T * m;                // 7.5
    const int    n = static_cast<int>(std::ceil(N));   // 8
    const double fracN = N - std::floor(N);            // 0.5
    const double alpha = (std::abs(fracN) < 1e-12) ? 0.0 : (1.0 - fracN); // 0.5

    const double dirty_explicit = dirty_coupon_price(P, c, y, n, m, alpha);
    const double clean_explicit = clean_coupon_price(P, c, y, n, m, alpha);
    const double dirty_T        = dirty_coupon_price_from_T(P, c, y, T, m);
    const double clean_T        = clean_coupon_price_from_T(P, c, y, T, m);

    REQUIRE(clean_explicit == Approx(dirty_explicit - accrued_interest(P, c, m, alpha)).epsilon(1e-12));
    REQUIRE(dirty_T == Approx(dirty_explicit).epsilon(1e-12));
    REQUIRE(clean_T == Approx(clean_explicit).epsilon(1e-12));
}

TEST_CASE("Dirty closed-form equals explicit sum for small n") {
    const double P = 1000.0, c = 0.03, y = 0.02; const int m = 2;
    const int n = 3;
    const double alpha = 0.4;

    const double dirty_cf = dirty_coupon_price(P, c, y, n, m, alpha);

    // Recompute via explicit sum from definition
    const double r = y / m;
    const double b = 1.0 + r;
    const double C = P * (c / m);

    double dirty_sum = 0.0;
    for (int k = 1; k <= n; ++k)
        dirty_sum += C / std::pow(b, static_cast<double>(k) - alpha);
    dirty_sum += P / std::pow(b, static_cast<double>(n) - alpha);

    REQUIRE(dirty_cf == Approx(dirty_sum).margin(1e-12));
}

TEST_CASE("Dirty price r -> 0 analytic limit") {
    const double P = 1000.0, c = 0.05; const int m = 2;
    const int n = 6;      // 3 years semiannual
    const double alpha = 0.3;
    const double y = 1e-16; // ~0

    const double dirty = dirty_coupon_price(P, c, y, n, m, alpha);
    const double C = P * (c / m);
    // Limit: C*n + P
    REQUIRE(dirty == Approx(C * n + P).margin(1e-10));
}