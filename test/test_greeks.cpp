//
// Created by Nikolay Tsonev on 09/11/2025.
//

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <limits>
#include <string>

#include "pyfi/option.h"

using namespace pyfi::option;

static double normal_pdf(double x) {
    const double PI = std::acos(-1.0);
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * PI);
}
static double normal_cdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

static double ref_d1(double S, double K, double sigma, double r, double q, double T) {
    return (std::log(S / K) + (r - q + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
}
static double ref_d2(double d1, double sigma, double T) {
    return d1 - sigma * std::sqrt(T);
}

// reference Greeks (continuous dividend q)
static double ref_call_delta(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    return std::exp(-q * T) * normal_cdf(d1);
}
static double ref_put_delta(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    return std::exp(-q * T) * (normal_cdf(d1) - 1.0);
}
static double ref_gamma(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    return std::exp(-q * T) * normal_pdf(d1) / (S * sigma * std::sqrt(T));
}
static double ref_vega(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    return S * std::exp(-q * T) * normal_pdf(d1) * std::sqrt(T);
}
static double ref_call_theta(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    double d2 = ref_d2(d1, sigma, T);
    double first = -(S * sigma * std::exp(-q * T) * normal_pdf(d1)) / (2.0 * std::sqrt(T));
    double second = -r * K * std::exp(-r * T) * normal_cdf(d2);
    double third = q * S * std::exp(-q * T) * normal_cdf(d1);
    return first + second + third; // per-year theta
}
static double ref_put_theta(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    double d2 = ref_d2(d1, sigma, T);
    double first = -(S * sigma * std::exp(-q * T) * normal_pdf(d1)) / (2.0 * std::sqrt(T));
    double second = r * K * std::exp(-r * T) * normal_cdf(-d2);
    double third = -q * S * std::exp(-q * T) * normal_cdf(-d1);
    return first + second + third; // per-year theta
}
static double ref_call_rho(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    double d2 = ref_d2(d1, sigma, T);
    return K * T * std::exp(-r * T) * normal_cdf(d2);
}
static double ref_put_rho(double S, double K, double sigma, double r, double q, double T) {
    double d1 = ref_d1(S, K, sigma, r, q, T);
    double d2 = ref_d2(d1, sigma, T);
    return -K * T * std::exp(-r * T) * normal_cdf(-d2);
}

struct Params {
    double S, K, sigma, r, q, T;
};

static Params cases[] = {{100.0, 100.0, 0.2, 0.05, 0.02, 1.0},
    {100.0, 110.0, 0.25, 0.03, 0.00, 0.5},
    {50.0, 40.0, 0.4, 0.01, 0.0, 2.0}};

TEST_CASE("Call user's bs_* functions and compare to reference math", "[bs][greeks]") {
    const double tol_rel = 1e-9; // adjust if needed
    for (auto p : cases) {
        std::string label =
            "S=" + std::to_string(p.S) + " K=" + std::to_string(p.K) + " sigma=" + std::to_string(p.sigma);
        SECTION(label) {
            // compute reference
            double r_cd = ref_call_delta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_pd = ref_put_delta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_g = ref_gamma(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_v = ref_vega(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_ct = ref_call_theta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_pt = ref_put_theta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_cr = ref_call_rho(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double r_pr = ref_put_rho(p.S, p.K, p.sigma, p.r, p.q, p.T);

            // call user's functions (match signatures you provided)
            double u_cd = bs_call_delta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double u_pd = bs_put_delta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double u_g = bs_gamma(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double u_v = bs_vega(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double u_ct = bs_call_theta(p.S, p.K, p.sigma, p.r, p.q, p.T);
            double u_pt = bs_put_theta(p.S, p.K, p.sigma, p.r, p.q, p.T);

            // Note: your header declared bs_call_rho(...) and bs_put_rho(...) without dividend_yield.
            double u_cr = bs_call_rho(p.S, p.K, p.sigma, p.r, p.T);
            double u_pr = bs_put_rho(p.S, p.K, p.sigma, p.r, p.T);

            // comparisons (relative)
            REQUIRE(u_cd == Catch::Approx(r_cd).epsilon(tol_rel));
            REQUIRE(u_pd == Catch::Approx(r_pd).epsilon(tol_rel));
            REQUIRE(u_g == Catch::Approx(r_g).epsilon(tol_rel));
            REQUIRE(u_v == Catch::Approx(r_v).epsilon(tol_rel));
            REQUIRE(u_ct == Catch::Approx(r_ct).epsilon(tol_rel));
            REQUIRE(u_pt == Catch::Approx(r_pt).epsilon(tol_rel));
            REQUIRE(u_cr == Catch::Approx(r_cr).epsilon(tol_rel));
            REQUIRE(u_pr == Catch::Approx(r_pr).epsilon(tol_rel));
        }
    }
}
