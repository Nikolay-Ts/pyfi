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

  double black_scholes_price(
    const double stock_price,
    const double strike_price,
    const double volotility,
    const double risk_free_rate,
    const uint64_t time
  ) {
    // compute our x
    const auto numerator = std::log(stock_price/strike_price) + (risk_free_rate + (std::pow(volotility, 2) /2)) * static_cast<double>(time);
    const auto x = numerator / (volotility * std::sqrt(time));

    const auto s_phi = stock_price * Phi(x);
    const auto k_phi = strike_price * std::exp(-(risk_free_rate * time)) * Phi(x - volotility*std::sqrt(time));

    return s_phi - k_phi;
  }


} //namespace pyfi::option