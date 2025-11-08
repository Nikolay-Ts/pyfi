#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "pyfi/bond.h"
#include <cmath>
using Catch::Approx;

using namespace pyfi::bond;

    TEST_CASE("Zero-coupon (discrete) matches textbook", "[bond][zero]") {
        const double F=1000, y=0.06, T=5; const int m=2;
        REQUIRE(zero_coupon_price(F,y,T,m)
                == Catch::Approx(F / std::pow(1.0 + y/m, m*T)).epsilon(1e-12));
    }

    TEST_CASE("Coupon bond: par when coupon==yield", "[bond][coupon]") {
        const double F=1000, c=0.05, y=0.05, T=10; const int m=2;
        REQUIRE(coupon_bond_price(F,c,y,T,m) == Catch::Approx(F).epsilon(1e-12));
    }

    TEST_CASE("Coupon bond: premium/discount", "[bond][coupon]") {
        const double F=1000, T=7; const int m=2;
        const double prem = coupon_bond_price(F, 0.07, 0.05, T, m);
        const double disc = coupon_bond_price(F, 0.03, 0.05, T, m);
        REQUIRE(prem > F);
        REQUIRE(disc < F);
    }

    static_assert([]{
        constexpr double z = zero_coupon_price_cexpr(1000.0, 0.05, 5, 2);
        return z > 770.0 && z < 790.0;
    }(), "constexpr zero_coupon_price_cexpr failed");

    static_assert([]{
        constexpr double p = coupon_bond_price_cexpr(1000.0, 0.05, 0.05, 10, 2);
        return p > 999.999 && p < 1000.001;
    }(), "constexpr coupon_bond_price_cexpr failed");
