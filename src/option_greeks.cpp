//
// Created by Nikolay Tsonev on 09/11/2025.
//

#include <math.h>
#include "../include/pyfi/option.h"

namespace pyfi::option {

    double custom_normal(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {
        const auto one_pi = 1.0 / (std::pow(M_PI * 2, 1 / 2));
        const auto x = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        const auto exp = std::exp(std::pow(-x, 2) / 2);

        return one_pi * exp;
    }

    double custom_normal(const double x) {
        const auto one_pi = 1.0 / (std::pow(M_PI * 2, 1 / 2));
        const auto exp = std::exp(std::pow(-x, 2) / 2);
        return one_pi * exp;
    }

    double bs_call_delta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto n_x = custom_normal(stock_price, strike_price, volatility, risk_free_rate, time);
        return std::exp(-(time * dividend_yield)) * n_x;
    }

    double bs_put_delta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto n_x = custom_normal(stock_price, strike_price, volatility, risk_free_rate, time);
        return std::exp(-dividend_yield * time) * (n_x - 1.0);
    }

    double bs_gamma(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto frac = std::exp(-(time * dividend_yield)) / (stock_price * volatility * std::pow(time, 1 / 2));
        const auto n_x = custom_normal(stock_price, strike_price, volatility, risk_free_rate, time);
        return frac * n_x;
    }

    double bs_call_theta(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double dividend_yield,
        const double time) {

        const auto x1 = black_scholes_x(stock_price, strike_price, volatility, risk_free_rate, time);
        const auto x2 = x1 - volatility * std::pow(time, 1 / 2);
        const auto n_x1 = custom_normal(x1);
        const auto n_x2 = custom_normal(x2);

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
        const auto n_x1 = custom_normal(x1);
        const auto n_x2 = custom_normal(x2);

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
        const auto n_x1 = custom_normal(stock_price, strike_price, volatility, risk_free_rate, time);
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
