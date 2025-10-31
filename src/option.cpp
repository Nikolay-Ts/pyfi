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

  double black_scholes_call(
    const double stock_price,
    const double strike_price,
    const double volatility,
    const double risk_free_rate,
    const double time
  ) {
    if (volatility < 1e-9 || time < 1e-9) {
      throw std::invalid_argument("Time or volatility cannot be zero");
    }

    // compute our x
    const auto numerator = std::log(stock_price/strike_price) + (risk_free_rate + (std::pow(volatility, 2) /2)) * static_cast<double>(time);
    const auto x = numerator / (volatility * std::sqrt(time));

    const auto s_phi = stock_price * Phi(x);
    const auto k_phi = strike_price * std::exp(-(risk_free_rate * time)) * Phi(x - volatility*std::sqrt(time));

    return s_phi - k_phi;
  }

  double black_scholes_put(
      const double stock_price,
      const double strike_price,
      const double volatility,
      const double risk_free_rate,
      const double time
  ) {
    const auto call_price = black_scholes_call(
      stock_price, strike_price, volatility, risk_free_rate, time
    );

    return call_price + (strike_price * std::exp(-(risk_free_rate*time))) - stock_price;
  }

  std::vector<double> call_payoff(const std::vector<double>& spot_rates, const double strike_price) {
    const auto vec_size = spot_rates.size();
    std::vector<double> maximum(vec_size);

    for (auto i = 0; i < vec_size; i++) {
      maximum[i] = std::max(spot_rates[i]-strike_price, 0.0);
    }

    return maximum;
  }

  std::vector<double> put_payoff(const std::vector<double>& spot_rates, const double strike_price) {
    const auto vec_size = spot_rates.size();
    std::vector<double> maximum(vec_size);

    for (auto i = 0; i < vec_size; i++) {
      maximum[i] = std::max(strike_price - spot_rates[i], 0.0);
    }

    return maximum;
  }


} //namespace pyfi::option