//
// Created by Bilal Waraich on 09/01/2026.
//

#ifndef BROWNIAN_H
#define BROWNIAN_H

#include <vector>
#include <functional>
#include <random>

namespace pyfi::brownian {

    /**
     * Seedable random number engine used across all Brownian-related models.
     */
    class RandomEngine {
    public:
        explicit RandomEngine(unsigned int seed = std::random_device{}());
        void reseed(unsigned int seed);
        std::mt19937& engine();
    private:
        std::mt19937 gen_;
    };

    /**
     * Simulate a single Brownian motion path.
     *
     * @param x0 initial value W₀
     * @param T total time horizon
     * @param steps number of discrete time steps
     * @param engine seeded random engine for reproducibility
     * @return vector of size steps + 1 representing the Brownian path
     * @throw std::invalid_argument if T <= 0 or steps <= 0
     */
    std::vector<double> simulate_brownian_motion(
        double x0,
        double T,
        int steps,
        RandomEngine& engine
    );

    /**
     * Simulate multiple independent Brownian paths.
     *
     * @param x0 initial value W₀
     * @param T total time horizon
     * @param steps number of discrete time steps per path
     * @param num_paths number of independent paths to generate
     * @param engine seeded random engine for reproducibility
     * @return vector of Brownian paths
     * @throw std::invalid_argument if num_paths <= 0
     */
    std::vector<std::vector<double>> simulate_brownian_paths(
        double x0,
        double T,
        int steps,
        int num_paths,
        RandomEngine& engine
    );

    /**
     * Simulate a single Geometric Brownian Motion price path.
     *
     * Uses the closed-form solution:
     *   S_{t+Δt} = S_t · exp((μ − ½σ²)Δt + σ√Δt·Z)
     *
     * @param mu drift coefficient (mean return)
     * @param sigma volatility coefficient
     * @param s0 initial asset value S₀
     * @param T total time horizon
     * @param steps number of discrete time steps
     * @param engine seeded random engine for reproducibility
     * @return vector of simulated asset prices
     * @throw std::invalid_argument if T <= 0 or steps <= 0
     */
    std::vector<double> simulate_gbm(
        double mu,
        double sigma,
        double s0,
        double T,
        int steps,
        RandomEngine& engine
    );

    /**
     * Return the mean (drift) of a GBM process.
     * 
     * @param mu drift coefficient
     * @return drift μ
     */
    [[nodiscard]] double gbm_mean(double mu);

    /**
     * Return the variance of a GBM process.
     * 
     * @param sigma volatility coefficient
     * @return variance σ²
     */
    [[nodiscard]] double gbm_variance(double sigma);

    /**
     * Simulate a stochastic differential equation using Euler–Maruyama.
     *
     * Solves SDEs of the form: dX_t = a(X_t, t) dt + b(X_t, t) dW_t
     *
     * @param drift drift function a(x, t)
     * @param diffusion diffusion function b(x, t)
     * @param x0 initial value
     * @param T total time horizon
     * @param steps number of discrete time steps
     * @param engine seeded random engine for reproducibility
     * @return vector representing the simulated path
     * @throw std::invalid_argument if T <= 0 or steps <= 0
     */
    std::vector<double> euler_maruyama(
        std::function<double(double, double)> drift,
        std::function<double(double, double)> diffusion,
        double x0,
        double T,
        int steps,
        RandomEngine& engine
    );

} // namespace pyfi::brownian

#endif // BROWNIAN_H
