//
// Created by Nikolay Tsonev on 21/11/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "../include/pyfi/option.h"
#include "option_bind.h"

#include <stdexcept>
#include <string>

namespace py = pybind11;

void add_option_module(py::module_& m) {
    using namespace pyfi::option;

    m.def("black_scholes_x",
        &black_scholes_x,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        py::arg_v("yield_curve", 0.0, "0.0"),
        R"doc(
         black_scholes_x(
             stock_price: float,
             strike_price: float,
             volatility: float,
             risk_free_rate: float,
             time: float,
             yield_curve: float = 0.0
         ) -> float

         Internal helper computing the Black–Scholes x1 (a.k.a. d1).

         Parameters
         ----------
         stock_price :
             Current underlying spot price S.
         strike_price :
             Strike price K.
         volatility :
             Volatility σ (annualized).
         risk_free_rate :
             Risk-free rate r (continuously compounded).
         time :
             Time to maturity T (in years).
         yield_curve :
             Continuous dividend yield q or carry, default 0.0.

         Returns
         -------
         float
             x1 (d1 in Black–Scholes notation).

         Notes
         -----
         Caller must ensure `time > 0` and `volatility > 0`.
         )doc");

    m.def("black_scholes_call",
        &black_scholes_call,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        py::arg_v("yield_curve", 0.0, "0.0"),
        R"doc(
        black_scholes_call(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            time: float,
            yield_curve: float = 0.0
        ) -> float

        European call option price under the Black–Scholes model.

        Parameters
        ----------
        stock_price :
            Current underlying spot price S.
        strike_price :
            Strike price K.
        volatility :
            Volatility σ (annualized).
        risk_free_rate :
            Risk-free rate r (continuously compounded).
        time :
            Time to maturity T (in years).
        yield_curve :
            Continuous dividend yield q or carry, default 0.0.

        Raises
        ------
        ValueError
            If `time <= 0` or `volatility <= 0` in the underlying C++ code.
        )doc");

    m.def("black_scholes_put",
        &black_scholes_put,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        py::arg_v("yield_curve", 0.0, "0.0"),
        R"doc(
        black_scholes_put(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            time: float,
            yield_curve: float = 0.0
        ) -> float

        European put option price, derived from Black–Scholes using put–call parity.

        Parameters
        ----------
        stock_price :
            Current underlying spot price S.
        strike_price :
            Strike price K.
        volatility :
            Volatility σ (annualized).
        risk_free_rate :
            Risk-free rate r (continuously compounded).
        time :
            Time to maturity T (in years).
        yield_curve :
            Continuous dividend yield q or carry, default 0.0.

        Raises
        ------
        ValueError
            If `time <= 0` or `volatility <= 0` in the underlying C++ code.
        )doc");

    m.def("norm_pdf",
        static_cast<double (*)(double)>(&norm_pdf),
        py::arg("x"),
        R"doc(
        norm_pdf(x: float) -> float

        Standard normal probability density function.

        Parameters
        ----------
        x :
            Real input.

        Returns
        -------
        float
            ϕ(x) = (1 / sqrt(2π)) * exp(-x² / 2).
        )doc");

    m.def("norm_pdf",
        static_cast<double (*)(double, double, double, double, double)>(&norm_pdf),
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        R"doc(
        norm_pdf(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            time: float
        ) -> float

        Convenience wrapper around the normal PDF in a Black–Scholes setting.

        Parameters
        ----------
        stock_price :
            Spot price S.
        strike_price :
            Strike K.
        volatility :
            Volatility σ.
        risk_free_rate :
            Risk-free rate r.
        time :
            Time to maturity T.
        )doc");


    m.def("bs_call_delta",
        &bs_call_delta,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_call_delta(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Delta of a European call option (∂C/∂S).

        Parameters
        ----------
        stock_price :
            Spot S.
        strike_price :
            Strike K.
        volatility :
            Volatility σ.
        risk_free_rate :
            Risk-free rate r.
        dividend_yield :
            Continuous dividend yield q.
        time :
            Time to maturity T.
        )doc");

    m.def("bs_put_delta",
        &bs_put_delta,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_put_delta(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Delta of a European put option (∂P/∂S).
        )doc");

    m.def("bs_gamma",
        &bs_gamma,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_gamma(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Gamma of European options (second derivative w.r.t spot).

        Notes
        -----
        The same formula applies to both calls and puts.
        )doc");

    m.def("bs_call_theta",
        &bs_call_theta,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_call_theta(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Theta of a European call option (∂C/∂t).
        )doc");

    m.def("bs_put_theta",
        &bs_put_theta,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_put_theta(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Theta of a European put option (∂P/∂t).
        )doc");

    m.def("bs_vega",
        &bs_vega,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("dividend_yield"),
        py::arg("time"),
        R"doc(
        bs_vega(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            dividend_yield: float,
            time: float
        ) -> float

        Vega of European options (∂V/∂σ), same for calls and puts.
        )doc");

    m.def("bs_rho_calculation",
        &bs_rho_calculation,
        py::arg("strike_price"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        py::arg("x2"),
        R"doc(
        bs_rho_calculation(
            strike_price: float,
            risk_free_rate: float,
            time: float,
            x2: float
        ) -> float

        Internal helper for rho, shared by call and put versions.

        Parameters
        ----------
        strike_price :
            Strike K.
        risk_free_rate :
            Rate r.
        time :
            Time to maturity T.
        x2 :
            x2 (d2) or -x2 depending on call/put.
        )doc");

    m.def("bs_call_rho",
        &bs_call_rho,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        R"doc(
        bs_call_rho(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            time: float
        ) -> float

        Rho of a European call option (∂C/∂r).
        )doc");

    m.def("bs_put_rho",
        &bs_put_rho,
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        R"doc(
        bs_put_rho(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            time: float
        ) -> float

        Rho of a European put option (∂P/∂r).
        )doc");

    // Wrapper for binomial_eu_option that takes a string for payoff type
    m.def("binomial_eu_option",
        [](double stock_price,
           double strike_price,
           double volatility,
           double risk_free_rate,
           int steps,
           double time,
           const std::string& payoff_type) -> double {
            payoff_func payoff;
            if (payoff_type == "call") {
                payoff = call_payoff;
            } else if (payoff_type == "put") {
                payoff = put_payoff;
            } else {
                throw std::invalid_argument("payoff_type must be 'call' or 'put'");
            }
            return binomial_eu_option(stock_price, strike_price, volatility,
                                      risk_free_rate, steps, time, payoff);
        },
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("steps"),
        py::arg("time"),
        py::arg("payoff_type"),
        R"doc(
        binomial_eu_option(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            steps: int,
            time: float,
            payoff_type: str
        ) -> float

        European option price using a binomial tree.

        Parameters
        ----------
        stock_price :
            Spot price S0.
        strike_price :
            Strike K.
        volatility :
            Volatility σ.
        risk_free_rate :
            Risk-free rate r (continuously compounded).
        steps :
            Number of steps in the binomial tree.
        time :
            Time to maturity T.
        payoff_type :
            Either "call" or "put".

        Raises
        ------
        ValueError
            If payoff_type is not "call" or "put".
        )doc");

    // Wrapper for binomial_us_option that takes a string for payoff type
    m.def("binomial_us_option",
        [](double stock_price,
           double strike_price,
           double volatility,
           double risk_free_rate,
           int steps,
           double time,
           const std::string& payoff_type) -> double {
            payoff_func payoff;
            if (payoff_type == "call") {
                payoff = call_payoff;
            } else if (payoff_type == "put") {
                payoff = put_payoff;
            } else {
                throw std::invalid_argument("payoff_type must be 'call' or 'put'");
            }
            return binomial_us_option(stock_price, strike_price, volatility,
                                      risk_free_rate, steps, time, payoff);
        },
        py::arg("stock_price"),
        py::arg("strike_price"),
        py::arg("volatility"),
        py::arg("risk_free_rate"),
        py::arg("steps"),
        py::arg("time"),
        py::arg("payoff_type"),
        R"doc(
        binomial_us_option(
            stock_price: float,
            strike_price: float,
            volatility: float,
            risk_free_rate: float,
            steps: int,
            time: float,
            payoff_type: str
        ) -> float

        American option price using a binomial tree with early exercise.

        Parameters
        ----------
        stock_price :
            Spot price S0.
        strike_price :
            Strike K.
        volatility :
            Volatility σ.
        risk_free_rate :
            Risk-free rate r (continuously compounded).
        steps :
            Number of steps in the binomial tree.
        time :
            Time to maturity T.
        payoff_type :
            Either "call" or "put".

        Raises
        ------
        ValueError
            If payoff_type is not "call" or "put".
        )doc");

    m.def("forward_from_yield",
        &forward_from_yield,
        py::arg("spot_price"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        py::arg_v("dividend_yield", 0.0, "0.0"),
        R"doc(
        forward_from_yield(
            spot_price: float,
            risk_free_rate: float,
            time: float,
            dividend_yield: float = 0.0
        ) -> float

        Forward price under continuous compounding:

            F0 = S0 * exp((r - q) * T)

        Parameters
        ----------
        spot_price :
            Current spot price S0.
        risk_free_rate :
            Continuously compounded risk-free rate r.
        time :
            Time to maturity T (years).
        dividend_yield :
            Continuous dividend yield q.

        Raises
        ------
        ValueError
            If `time <= 0` or `spot_price <= 0` in the underlying C++ code.
        )doc");

    m.def("yield_from_forward",
        &yield_from_forward,
        py::arg("spot_price"),
        py::arg("forward_price"),
        py::arg("risk_free_rate"),
        py::arg("time"),
        R"doc(
        yield_from_forward(
            spot_price: float,
            forward_price: float,
            risk_free_rate: float,
            time: float
        ) -> float

        Implied continuous dividend yield from a forward price:

            q = r - (1/T) * ln(F0 / S0)

        Parameters
        ----------
        spot_price :
            Spot price S0.
        forward_price :
            Forward price F0.
        risk_free_rate :
            Continuously compounded risk-free rate r.
        time :
            Time to maturity T (years).

        Raises
        ------
        ValueError
            If `time <= 0` or `spot_price <= 0` or `forward_price <= 0`
            in the underlying C++ code.
        )doc");
}