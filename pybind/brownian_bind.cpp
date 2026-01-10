//
// Created by Bilal Waraich on 10/01/2026.
//

#include "brownian_bind.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "../include/pyfi/brownian.h"

namespace py = pybind11;

void add_brownian_module(py::module_& m) {
    using namespace pyfi::brownian;

    // RandomEngine class
    py::class_<RandomEngine>(m, "RandomEngine", R"doc(
        Seedable random number engine for reproducible stochastic simulations.

        Wraps a Mersenne Twister (std::mt19937) to guarantee deterministic
        behavior when a fixed seed is supplied. This engine is passed to
        all Brownian-related simulation functions.

        Examples
        --------
        >>> import pyfi
        >>> rng = pyfi.brownian.RandomEngine(42)
        >>> path = pyfi.brownian.simulate_brownian_motion(0.0, 1.0, 100, rng)
        )doc")
        .def(py::init<>(),
            R"doc(
            RandomEngine()

            Construct with a random seed from std::random_device.
            )doc")
        .def(py::init<unsigned int>(),
            py::arg("seed"),
            R"doc(
            RandomEngine(seed: int)

            Construct with an explicit seed for reproducibility.

            Parameters
            ----------
            seed : int
                Seed value for the PRNG.
            )doc")
        .def("reseed",
            &RandomEngine::reseed,
            py::arg("seed"),
            R"doc(
            reseed(seed: int) -> None

            Reset the random number generator with a new seed.

            Parameters
            ----------
            seed : int
                New seed value.
            )doc");

    // simulate_brownian_motion
    m.def("simulate_brownian_motion",
        &simulate_brownian_motion,
        py::arg("x0"),
        py::arg("T"),
        py::arg("steps"),
        py::arg("engine"),
        R"doc(
        simulate_brownian_motion(
            x0: float,
            T: float,
            steps: int,
            engine: RandomEngine
        ) -> list[float]

        Simulate a single Brownian motion (Wiener process) path.

        Models the stochastic process dW_t ~ Normal(0, dt) with independent
        Gaussian increments, starting at x0 and evolving over time horizon T.

        Parameters
        ----------
        x0 : float
            Initial value W₀.
        T : float
            Total time horizon.
        steps : int
            Number of discrete time steps.
        engine : RandomEngine
            Seeded random engine for reproducibility.

        Returns
        -------
        list[float]
            Vector of size steps + 1 representing the Brownian path.

        Raises
        ------
        ValueError
            If T <= 0 or steps <= 0.

        Examples
        --------
        >>> import pyfi
        >>> rng = pyfi.brownian.RandomEngine(42)
        >>> path = pyfi.brownian.simulate_brownian_motion(0.0, 1.0, 100, rng)
        >>> len(path)
        101
        )doc");

    // simulate_brownian_paths
    m.def("simulate_brownian_paths",
        &simulate_brownian_paths,
        py::arg("x0"),
        py::arg("T"),
        py::arg("steps"),
        py::arg("num_paths"),
        py::arg("engine"),
        R"doc(
        simulate_brownian_paths(
            x0: float,
            T: float,
            steps: int,
            num_paths: int,
            engine: RandomEngine
        ) -> list[list[float]]

        Simulate multiple independent Brownian motion paths.

        Each path uses the same random engine instance and advances it
        sequentially; reproducibility is preserved under a fixed seed.

        Parameters
        ----------
        x0 : float
            Initial value W₀.
        T : float
            Total time horizon.
        steps : int
            Number of discrete time steps per path.
        num_paths : int
            Number of independent paths to generate.
        engine : RandomEngine
            Seeded random engine for reproducibility.

        Returns
        -------
        list[list[float]]
            Vector of Brownian paths, each of size steps + 1.

        Raises
        ------
        ValueError
            If num_paths <= 0.

        Examples
        --------
        >>> import pyfi
        >>> rng = pyfi.brownian.RandomEngine(42)
        >>> paths = pyfi.brownian.simulate_brownian_paths(0.0, 1.0, 100, 1000, rng)
        >>> len(paths)
        1000
        )doc");

    // simulate_gbm
    m.def("simulate_gbm",
        &simulate_gbm,
        py::arg("mu"),
        py::arg("sigma"),
        py::arg("s0"),
        py::arg("T"),
        py::arg("steps"),
        py::arg("engine"),
        R"doc(
        simulate_gbm(
            mu: float,
            sigma: float,
            s0: float,
            T: float,
            steps: int,
            engine: RandomEngine
        ) -> list[float]

        Simulate a Geometric Brownian Motion (GBM) price path.

        Models the stochastic differential equation:
            dS_t = μ S_t dt + σ S_t dW_t

        Uses the exact discretized solution:
            S_{t+Δt} = S_t · exp((μ − ½σ²)Δt + σ√Δt·Z)

        where Z ~ N(0,1). Commonly used for asset price dynamics in
        quantitative finance.

        Parameters
        ----------
        mu : float
            Drift coefficient (mean return).
        sigma : float
            Volatility coefficient (standard deviation).
        s0 : float
            Initial asset value S₀.
        T : float
            Total time horizon.
        steps : int
            Number of discrete time steps.
        engine : RandomEngine
            Seeded random engine for reproducibility.

        Returns
        -------
        list[float]
            Vector of simulated asset prices of size steps + 1.

        Raises
        ------
        ValueError
            If T <= 0 or steps <= 0.

        Examples
        --------
        >>> import pyfi
        >>> rng = pyfi.brownian.RandomEngine(42)
        >>> # Simulate stock price: 5% drift, 20% volatility, $100 initial
        >>> path = pyfi.brownian.simulate_gbm(0.05, 0.20, 100.0, 1.0, 252, rng)
        >>> path[0]
        100.0
        )doc");

    // gbm_mean
    m.def("gbm_mean",
        &gbm_mean,
        py::arg("mu"),
        R"doc(
        gbm_mean(mu: float) -> float

        Return the drift (mean) parameter μ of a GBM process.

        Parameters
        ----------
        mu : float
            Drift coefficient.

        Returns
        -------
        float
            The drift μ.
        )doc");

    // gbm_variance
    m.def("gbm_variance",
        &gbm_variance,
        py::arg("sigma"),
        R"doc(
        gbm_variance(sigma: float) -> float

        Return the variance σ² of a GBM process.

        Parameters
        ----------
        sigma : float
            Volatility coefficient.

        Returns
        -------
        float
            The variance σ².
        )doc");

    // euler_maruyama
    m.def("euler_maruyama",
        &euler_maruyama,
        py::arg("drift"),
        py::arg("diffusion"),
        py::arg("x0"),
        py::arg("T"),
        py::arg("steps"),
        py::arg("engine"),
        R"doc(
        euler_maruyama(
            drift: Callable[[float, float], float],
            diffusion: Callable[[float, float], float],
            x0: float,
            T: float,
            steps: int,
            engine: RandomEngine
        ) -> list[float]

        Simulate a stochastic differential equation using Euler–Maruyama discretization.

        Solves SDEs of the form:
            dX_t = a(X_t, t) dt + b(X_t, t) dW_t

        using first-order time discretization:
            X_{t+Δt} = X_t + a(X_t,t)·Δt + b(X_t,t)·√Δt·Z

        where Z ~ N(0,1).

        Parameters
        ----------
        drift : callable
            Drift function a(x, t) taking current state and time.
        diffusion : callable
            Diffusion function b(x, t) taking current state and time.
        x0 : float
            Initial value.
        T : float
            Total time horizon.
        steps : int
            Number of discrete time steps.
        engine : RandomEngine
            Seeded random engine for reproducibility.

        Returns
        -------
        list[float]
            Vector representing the simulated path of size steps + 1.

        Raises
        ------
        ValueError
            If T <= 0 or steps <= 0.

        Examples
        --------
        >>> import pyfi
        >>> rng = pyfi.brownian.RandomEngine(42)
        >>> # Ornstein-Uhlenbeck process: dX = -0.5(X - 2)dt + 0.1dW
        >>> drift = lambda x, t: -0.5 * (x - 2.0)
        >>> diffusion = lambda x, t: 0.1
        >>> path = pyfi.brownian.euler_maruyama(drift, diffusion, 0.0, 10.0, 1000, rng)
        )doc");
}