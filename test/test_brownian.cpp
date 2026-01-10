//
// Created by Bilal Waraich on 09/01/2026.
//

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <numeric>
#include <vector>

#include "pyfi/brownian.h"

using namespace pyfi::brownian;
using namespace Catch;


TEST_CASE("RandomEngine: seeding produces deterministic results") {
    RandomEngine rng1(42);
    
    std::normal_distribution<double> dist(0.0, 1.0);
    
    std::vector<double> samples1;
    for (int i = 0; i < 100; ++i) {
        samples1.push_back(dist(rng1.engine()));
    }
    
    // Reset and generate again
    RandomEngine rng2(42);
    std::vector<double> samples2;
    for (int i = 0; i < 100; ++i) {
        samples2.push_back(dist(rng2.engine()));
    }

    REQUIRE(samples1.size() == samples2.size());
    for (size_t i = 0; i < samples1.size(); ++i) {
        REQUIRE(samples1[i] == Approx(samples2[i]).epsilon(1e-15));
    }
}

TEST_CASE("RandomEngine: reseed resets sequence") {
    RandomEngine rng(42);
    std::normal_distribution<double> dist1(0.0, 1.0);

    const double first = dist1(rng.engine());

    rng.reseed(42);
    std::normal_distribution<double> dist2(0.0, 1.0);
    const double second = dist2(rng.engine());

    REQUIRE(first == Approx(second).epsilon(1e-15));
}

TEST_CASE("RandomEngine: reseed with different seed changes sequence") {
    RandomEngine rng(42);
    std::normal_distribution<double> dist(0.0, 1.0);

    const double first = dist(rng.engine());

    rng.reseed(1337);
    const double second = dist(rng.engine());

    REQUIRE(first != second);
}

TEST_CASE("Brownian motion: path starts at x0") {
    RandomEngine rng(123);
    const double x0 = 5.0;

    auto path = simulate_brownian_motion(x0, 1.0, 100, rng);

    REQUIRE(path.size() == 101);
    REQUIRE(path[0] == Approx(x0).epsilon(1e-15));
}

TEST_CASE("Brownian motion: returns correct path length") {
    RandomEngine rng(456);

    for (int steps : {10, 50, 100, 1000}) {
        auto path = simulate_brownian_motion(0.0, 1.0, steps, rng);
        REQUIRE(path.size() == steps + 1);
    }
}

TEST_CASE("Brownian motion: reproducibility with same seed") {
    const double x0 = 0.0, T = 1.0;
    const int steps = 100;

    RandomEngine rng1(777);
    auto path1 = simulate_brownian_motion(x0, T, steps, rng1);

    RandomEngine rng2(777);
    auto path2 = simulate_brownian_motion(x0, T, steps, rng2);

    REQUIRE(path1.size() == path2.size());
    for (size_t i = 0; i < path1.size(); ++i) {
        REQUIRE(path1[i] == Approx(path2[i]).epsilon(1e-15));
    }
}

TEST_CASE("Brownian motion: mean approximately zero (statistical)") {
    RandomEngine rng(999);
    const int num_paths = 5000;
    const double T = 1.0;
    const int steps = 100;

    auto paths = simulate_brownian_paths(0.0, T, steps, num_paths, rng);

    double mean = 0.0;
    for (const auto& p : paths) mean += p.back();
    mean /= num_paths;

    REQUIRE(std::abs(mean) < 0.05);
}

TEST_CASE("Brownian motion: variance approximately T (statistical)") {
    RandomEngine rng(1234);
    const int num_paths = 5000;
    const double T = 1.0;
    const int steps = 100;

    auto paths = simulate_brownian_paths(0.0, T, steps, num_paths, rng);

    double mean = 0.0;
    for (const auto& p : paths) mean += p.back();
    mean /= num_paths;

    double var = 0.0;
    for (const auto& p : paths) {
        var += (p.back() - mean) * (p.back() - mean);
    }
    var /= (num_paths - 1);

    REQUIRE(var == Approx(T).epsilon(0.05));
}

TEST_CASE("Brownian motion: invalid inputs throw") {
    RandomEngine rng(42);

    REQUIRE_THROWS(simulate_brownian_motion(0.0, -1.0, 100, rng));
    REQUIRE_THROWS(simulate_brownian_motion(0.0, 1.0, 0, rng));
}

TEST_CASE("Brownian paths: correct number of paths generated") {
    RandomEngine rng(42);

    for (int num_paths : {1, 10, 100}) {
        auto paths = simulate_brownian_paths(0.0, 1.0, 50, num_paths, rng);
        REQUIRE(paths.size() == num_paths);
        for (const auto& path : paths) {
            REQUIRE(path.size() == 51);
        }
    }
}

TEST_CASE("Brownian paths: invalid num_paths throws") {
    RandomEngine rng(42);

    REQUIRE_THROWS(simulate_brownian_paths(0.0, 1.0, 100, 0, rng));
    REQUIRE_THROWS(simulate_brownian_paths(0.0, 1.0, 100, -5, rng));
}

TEST_CASE("GBM: path starts at s0") {
    RandomEngine rng(42);
    const double s0 = 100.0;

    auto path = simulate_gbm(0.05, 0.2, s0, 1.0, 252, rng);

    REQUIRE(path.size() == 253);
    REQUIRE(path.front() == Approx(s0).epsilon(1e-15));
}

TEST_CASE("GBM: path values always positive") {
    RandomEngine rng(567);

    auto path = simulate_gbm(0.05, 0.3, 100.0, 1.0, 252, rng);

    for (double val : path) {
        REQUIRE(val > 0.0);
    }
}

TEST_CASE("GBM: reproducibility with same seed") {
    const double mu = 0.05, sigma = 0.2, s0 = 100.0;
    const double T = 1.0;
    const int steps = 100;

    RandomEngine rng1(888);
    auto path1 = simulate_gbm(mu, sigma, s0, T, steps, rng1);

    RandomEngine rng2(888);
    auto path2 = simulate_gbm(mu, sigma, s0, T, steps, rng2);

    REQUIRE(path1.size() == path2.size());
    for (size_t i = 0; i < path1.size(); ++i) {
        REQUIRE(path1[i] == Approx(path2[i]).epsilon(1e-15));
    }
}

TEST_CASE("GBM: zero volatility gives deterministic drift") {
    RandomEngine rng(42);
    const double mu = 0.05, sigma = 0.0, s0 = 100.0;
    const double T = 1.0;

    auto path = simulate_gbm(mu, sigma, s0, T, 100, rng);

    REQUIRE(path.back() == Approx(s0 * std::exp(mu * T)).epsilon(1e-12));
}

TEST_CASE("GBM: mean drift approximately correct (statistical)") {
    RandomEngine rng(2025);
    const double mu = 0.1, sigma = 0.2, s0 = 100.0;
    const double T = 1.0;
    const int steps = 252;
    const int num_paths = 3000;

    double mean_terminal = 0.0;

    for (int i = 0; i < num_paths; ++i) {
        auto path = simulate_gbm(mu, sigma, s0, T, steps, rng);
        mean_terminal += path.back();
    }
    mean_terminal /= num_paths;

    const double expected = s0 * std::exp(mu * T);

    REQUIRE(mean_terminal == Approx(expected).epsilon(0.05));
}

TEST_CASE("GBM: invalid inputs throw") {
    RandomEngine rng(42);

    REQUIRE_THROWS(simulate_gbm(0.05, 0.2, 100.0, -1.0, 100, rng));
    REQUIRE_THROWS(simulate_gbm(0.05, 0.2, 100.0, 1.0, 0, rng));
}

TEST_CASE("GBM: mean and variance functions") {
    REQUIRE(gbm_mean(0.05) == Approx(0.05).epsilon(1e-15));
    REQUIRE(gbm_mean(0.1) == Approx(0.1).epsilon(1e-15));

    REQUIRE(gbm_variance(0.2) == Approx(0.04).epsilon(1e-15));
    REQUIRE(gbm_variance(0.5) == Approx(0.25).epsilon(1e-15));
}

TEST_CASE("Euler-Maruyama: deterministic case (zero diffusion)") {
    RandomEngine rng(42);

    auto drift = [](double x, double) { return 0.1 * x; };
    auto diffusion = [](double, double) { return 0.0; };

    const double x0 = 1.0, T = 1.0;
    const int steps = 1000;

    auto path = euler_maruyama(drift, diffusion, x0, T, steps, rng);

    const double expected = x0 * std::exp(0.1 * T);

    REQUIRE(path.back() == Approx(expected).epsilon(0.01));
}

TEST_CASE("Euler-Maruyama: reproduces Brownian motion") {
    auto drift = [](double, double) { return 0.0; };
    auto diffusion = [](double, double) { return 1.0; };

    const double x0 = 0.0, T = 1.0;
    const int steps = 100;
    
    RandomEngine rng1(999);
    auto em_path = euler_maruyama(drift, diffusion, x0, T, steps, rng1);
    
    RandomEngine rng2(999);
    auto bm_path = simulate_brownian_motion(x0, T, steps, rng2);

    REQUIRE(em_path.size() == bm_path.size());
    for (size_t i = 0; i < em_path.size(); ++i) {
        REQUIRE(em_path[i] == Approx(bm_path[i]).epsilon(1e-14));
    }
}

TEST_CASE("Euler-Maruyama: Ornstein-Uhlenbeck mean reversion") {
    RandomEngine rng(12345);

    const double theta = 0.5, mu = 2.0, sigma = 0.1;
    auto drift = [theta, mu](double x, double) {
        return -theta * (x - mu);
    };
    auto diffusion = [sigma](double, double) {
        return sigma;
    };

    const double x0 = 0.0, T = 10.0;
    const int steps = 1000;

    auto path = euler_maruyama(drift, diffusion, x0, T, steps, rng);

    const double terminal = path.back();

    REQUIRE(std::abs(terminal - mu) < 1.0);
}

TEST_CASE("Euler-Maruyama: path starts at x0") {
    RandomEngine rng(42);

    auto drift = [](double x, double) { return 0.05 * x; };
    auto diffusion = [](double x, double) { return 0.2 * x; };

    const double x0 = 5.0;
    auto path = euler_maruyama(drift, diffusion, x0, 1.0, 100, rng);

    REQUIRE(path.size() == 101);
    REQUIRE(path[0] == Approx(x0).epsilon(1e-15));
}

TEST_CASE("Euler-Maruyama: reproducibility with same seed") {
    auto drift = [](double x, double) { return 0.1 * x; };
    auto diffusion = [](double x, double) { return 0.3 * x; };

    const double x0 = 1.0, T = 1.0;
    const int steps = 100;

    RandomEngine rng1(777);
    auto path1 = euler_maruyama(drift, diffusion, x0, T, steps, rng1);

    RandomEngine rng2(777);
    auto path2 = euler_maruyama(drift, diffusion, x0, T, steps, rng2);

    REQUIRE(path1.size() == path2.size());
    for (size_t i = 0; i < path1.size(); ++i) {
        REQUIRE(path1[i] == Approx(path2[i]).epsilon(1e-15));
    }
}

TEST_CASE("Euler-Maruyama: invalid inputs throw") {
    RandomEngine rng(42);

    auto drift = [](double, double) { return 0.0; };
    auto diffusion = [](double, double) { return 1.0; };

    REQUIRE_THROWS(euler_maruyama(drift, diffusion, 1.0, -1.0, 100, rng));
    REQUIRE_THROWS(euler_maruyama(drift, diffusion, 1.0, 1.0, 0, rng));
}

TEST_CASE("Euler-Maruyama: time-dependent drift and diffusion") {
    RandomEngine rng(42);

    auto drift = [](double, double t) { return t; };
    auto diffusion = [](double, double t) { return std::sqrt(std::max(t, 1e-10)); };

    const double x0 = 0.0, T = 1.0;
    const int steps = 1000;

    auto path = euler_maruyama(drift, diffusion, x0, T, steps, rng);

    REQUIRE(path.size() == 1001);
    REQUIRE(std::isfinite(path.back()));
}