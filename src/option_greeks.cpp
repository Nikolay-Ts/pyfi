//
// Created by Nikolay Tsonev on 09/11/2025.
//

#include <math.h>
#include "../include/pyfi/option.h"

namespace pyfi::option {

    double norm_pdf(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {
        const auto x = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        const double inv_sqrt_2pi = 1.0 / std::sqrt(2.0 * M_PI);
        return inv_sqrt_2pi * std::exp(-0.5 * x * x);
    }

    double norm_pdf(const double x) {
        const double inv_sqrt_2pi = 1.0 / std::sqrt(2.0 * M_PI);
        return inv_sqrt_2pi * std::exp(-0.5 * x * x);
    }

    inline double norm_cdf(const double x) {
        return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
    }

    double bs_call_delta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {
        const double d1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        return std::exp(-dividend_yield * time) * norm_cdf(d1);
    }

    double bs_put_delta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const double d1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        return std::exp(-dividend_yield * time) * norm_cdf(d1);
    }

    double bs_gamma(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const double d1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        return std::exp(-dividend_yield * time) * (norm_pdf(d1) - 1.0);
    }

    double bs_call_theta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto x1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        const auto x2 = x1 - volatility * std::pow(time, 1 / 2);
        const auto n_x1 = norm_pdf(x1);
        const auto n_x2 = norm_pdf(x2);

        const auto right_side = (risk_free_rate * stock_price * std::exp(-(risk_free_rate * time)) * n_x2) +
            (dividend_yield * strike_price * std::exp(-(dividend_yield * time)) * n_x1);

        const auto neg_fraction =
            -((stock_price * volatility * std::exp(-(dividend_yield * time))) / (2 * std::sqrt(time)) * n_x1);

        return 1.0 / time * (neg_fraction - right_side);
    }

    double bs_put_theta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto x1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        const auto x2 = x1 - volatility * std::pow(time, 1 / 2);
        const auto n_x1 = norm_pdf(x1);
        const auto n_x2 = norm_pdf(x2);

        const auto right_side = (risk_free_rate * stock_price * std::exp(-(risk_free_rate * time)) * n_x2) +
            (dividend_yield * strike_price * std::exp(-(dividend_yield * time)) * n_x1);

        const auto neg_fraction =
            -((stock_price * volatility * std::exp(-(dividend_yield * time))) / (2 * std::sqrt(time)) * n_x1);

        return 1.0 / time * (neg_fraction - right_side);
    }

    double bs_vega(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {
        const auto n_x1 = norm_pdf(stock_price, strike_price, volatility, risk_free_rate, time);
        const auto expo = std::exp(-(dividend_yield * time));

        return 1.0 / 100 * stock_price * expo * std::sqrt(time) * n_x1;
    }

    inline double
    bs_rho_calculation(const double strike_price, const double risk_free_rate, const double time, const double x2) {
        return 1.0 / 100 * strike_price * time * std::exp(-(risk_free_rate * time)) * x2;
    }

    double bs_call_rho(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {
        const auto x2 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time) -
            risk_free_rate * std::sqrt(time);

        return bs_rho_calculation(strike_price, risk_free_rate, time, x2);
    }

    double bs_put_rho(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {

        const auto x2 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time) -
            risk_free_rate * std::sqrt(time);

        return (-1 * bs_rho_calculation(strike_price, risk_free_rate, time, -x2));
    }

} // namespace pyfi::option
