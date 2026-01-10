//
// Created by Bilal Waraich on 09/01/2026.
//

#include "../include/pyfi/brownian.h"

#include <cmath>
#include <stdexcept>

namespace pyfi::brownian {

RandomEngine::RandomEngine(unsigned int seed)
    : gen_(seed) {}

void RandomEngine::reseed(unsigned int seed) {
    gen_.seed(seed);
}

std::mt19937& RandomEngine::engine() {
    return gen_;
}

std::vector<double> simulate_brownian_motion(
    double x0,
    double T,
    int steps,
    RandomEngine& engine
) {
    if (T <= 0.0 || steps <= 0) {
        throw std::invalid_argument(
            "simulate_brownian_motion: T and steps must be positive"
        );
    }

    const double dt = T / steps;
    std::normal_distribution<double> normal(0.0, 1.0);

    std::vector<double> path;
    path.reserve(steps + 1);
    path.push_back(x0);

    for (int i = 0; i < steps; ++i) {
        const double dW = std::sqrt(dt) * normal(engine.engine());
        path.push_back(path.back() + dW);
    }

    return path;
}

std::vector<std::vector<double>> simulate_brownian_paths(
    double x0,
    double T,
    int steps,
    int num_paths,
    RandomEngine& engine
) {
    if (num_paths <= 0) {
        throw std::invalid_argument(
            "simulate_brownian_paths: num_paths must be positive"
        );
    }

    std::vector<std::vector<double>> paths;
    paths.reserve(num_paths);

    for (int i = 0; i < num_paths; ++i) {
        paths.push_back(
            simulate_brownian_motion(x0, T, steps, engine)
        );
    }

    return paths;
}

std::vector<double> simulate_gbm(
    double mu,
    double sigma,
    double s0,
    double T,
    int steps,
    RandomEngine& engine
) {
    if (T <= 0.0 || steps <= 0) {
        throw std::invalid_argument(
            "simulate_gbm: T and steps must be positive"
        );
    }

    const double dt = T / steps;
    std::normal_distribution<double> normal(0.0, 1.0);

    std::vector<double> path;
    path.reserve(steps + 1);
    path.push_back(s0);

    for (int i = 0; i < steps; ++i) {
        const double Z = normal(engine.engine());
        const double prev = path.back();

        const double next = prev * std::exp(
            (mu - 0.5 * sigma * sigma) * dt
            + sigma * std::sqrt(dt) * Z
        );

        path.push_back(next);
    }

    return path;
}

double gbm_mean(double mu) {
    return mu;
}

double gbm_variance(double sigma) {
    return sigma * sigma;
}

std::vector<double> euler_maruyama(
    std::function<double(double, double)> drift,
    std::function<double(double, double)> diffusion,
    double x0,
    double T,
    int steps,
    RandomEngine& engine
) {
    if (T <= 0.0 || steps <= 0) {
        throw std::invalid_argument(
            "euler_maruyama: T and steps must be positive"
        );
    }

    const double dt = T / steps;
    std::normal_distribution<double> normal(0.0, 1.0);

    std::vector<double> path;
    path.reserve(steps + 1);
    path.push_back(x0);

    for (int i = 0; i < steps; ++i) {
        const double t = i * dt;
        const double Z = normal(engine.engine());

        const double prev = path.back();
        const double next = prev
            + drift(prev, t) * dt
            + diffusion(prev, t) * std::sqrt(dt) * Z;

        path.push_back(next);
    }

    return path;
}

} // namespace pyfi::brownian
