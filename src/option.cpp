//
// Created by Nikolay Tsonev on 30/10/2025.
//

#include "../include/pyfi/option.h"
#include <boost/math/distributions/normal.hpp>


namespace pyfi::option {


    inline double Phi(const double x) {
        static const boost::math::normal Z(0.0, 1.0);
        return boost::math::cdf(Z, x);
    }

    double black_scholes_call(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {
        if (volatility < 1e-9 || time < 1e-9) {
            throw std::invalid_argument("Time or volatility cannot be zero");
        }

        // compute our x
        const auto numerator = std::log(stock_price / strike_price) +
            (risk_free_rate + (std::pow(volatility, 2) / 2)) * static_cast<double>(time);
        const auto x = numerator / (volatility * std::sqrt(time));

        const auto s_phi = stock_price * Phi(x);
        const auto k_phi = strike_price * std::exp(-(risk_free_rate * time)) * Phi(x - volatility * std::sqrt(time));

        return s_phi - k_phi;
    }

    double black_scholes_put(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const double time) {
        if (volatility < 1e-9 || time < 1e-9) {
            throw std::invalid_argument("Time or volatility cannot be zero");
        }

        const double sqrtT = std::sqrt(time);
        const double vv = volatility * volatility;
        const double d1 =
            (std::log(stock_price / strike_price) + (risk_free_rate + 0.5 * vv) * time) / (volatility * sqrtT);
        const double d2 = d1 - volatility * sqrtT;

        const double pvK = strike_price * std::exp(-risk_free_rate * time);
        return pvK * Phi(-d2) - stock_price * Phi(-d1);
    }

    void call_payoff(std::vector<double>& spot_rates, const double strike_price) {
        const auto vec_size = spot_rates.size();
        for (size_t i = 0; i < vec_size; i++) {
            spot_rates[i] = std::max(spot_rates[i] - strike_price, 0.0);
        }
    }

    void put_payoff(std::vector<double>& spot_rates, const double strike_price) {
        const auto vec_size = spot_rates.size();
        for (size_t i = 0; i < vec_size; i++) {
            spot_rates[i] = std::max(strike_price - spot_rates[i], 0.0);
        }
    }

    std::vector<double> binomial_tree_setup(const double stock_price,
        const double strike_price,
        const double volatility,
        const int steps,
        const double time,
        const payoff_func payoff) {
        const auto dt = time / steps;
        const auto u = std::exp(volatility * std::sqrt(dt));
        const auto d = 1.0 / u;

        auto up = 1.0;
        auto dn = std::pow(d, steps);
        std::vector<double> options(steps + 1);

        for (int j = 0; j <= steps; ++j) {
            options[j] = stock_price * up * dn;
            up *= u;
            dn /= d;
        }

        payoff(options, strike_price);

        return options;
    }

    double binomial_eu_option(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const int steps,
        const double time,
        const payoff_func payoff) {
        const auto dt = time / steps;
        const auto u = std::exp(volatility * std::sqrt(dt));
        const auto d = 1.0 / u;
        const auto fair_prob = (std::exp(risk_free_rate * dt) - d) / (u - d);
        const auto discr = std::exp(-(risk_free_rate * dt));

        auto options = binomial_tree_setup(stock_price, strike_price, volatility, steps, time, payoff);

        for (int i = steps; i > 0; i--) {
            for (int j = 0; j < i; ++j) {
                options[j] = discr * (fair_prob * options[j + 1] + (1.0 - fair_prob) * options[j]);
            }
        }

        return options[0];
    }

    inline double eval_payoff_scalar(const double S, const double K, const payoff_func& payoff) {
        std::vector<double> tmp{S};
        payoff(tmp, K);
        return tmp[0];
    }

    double binomial_us_option(const double stock_price,
        const double strike_price,
        const double volatility,
        const double risk_free_rate,
        const int steps,
        const double time,
        const payoff_func payoff) {

        const auto dt = time / steps;
        const auto u = std::exp(volatility * std::sqrt(dt));
        const auto d = 1.0 / u;
        const auto fair_prob = (std::exp(risk_free_rate * dt) - d) / (u - d);
        const auto discr = std::exp(-(risk_free_rate * dt));

        auto options = binomial_tree_setup(stock_price, strike_price, volatility, steps, time, payoff);

        for (int i = steps - 1; i >= 0; --i) {
            for (int j = 0; j <= i; ++j) {
                const double cont = discr * (fair_prob * options[j + 1] + (1.0 - fair_prob) * options[j]);

                // node stock price S(i,j) = S0 * u^j * d^(i-j)
                const double Sij = stock_price * std::pow(u, j) * std::pow(d, i - j);

                const double early_exercise = eval_payoff_scalar(Sij, strike_price, payoff);
                options[j] = std::max(cont, early_exercise);
            }
        }

        return options[0];
    }


} // namespace pyfi::option
