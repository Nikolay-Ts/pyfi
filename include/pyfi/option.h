//
// Created by Nikolay Tsonev on 26/10/2025.
//

#ifndef OPTION_H
#define OPTION_H

#include <concepts>
#include <cmath>

namespace pyfi::option {

  /**
   *
   * Gives the probability of some normalised random variable x in a
   * Gaussian distr.
   *
   * @param x the normalised variable
   * @return the resolve of the integral of the gaussian distr.
   */
  inline double Phi(double x);

  /**
   *
   * Calculates the price of a European call option.
   *
   * @param stock_price
   * @param strike_price
   * @param volatility
   * @param risk_free_rate
   * @param time
   * @return price of the European call option
   * @throw std::invalid if time or volatility is 0
   */
  double black_scholes_call(
      double stock_price,
      double strike_price,
      double volatility,
      double risk_free_rate,
      double time
  );

/**
   *
   * Calculates the price of a European put option. The Black-Scholes model
   * does not do that on its own but using the put-call parity to derive the
   * EU put price.
   *
   * @param stock_price
   * @param strike_price
   * @param volatility
   * @param risk_free_rate
   * @param time in years
   * @return price of the European put option
   * @throw std::invalid if time or volatility is 0
   */
  double black_scholes_put(
      double stock_price,
      double strike_price,
      double volatility,
      double risk_free_rate,
      double time
  );

} // namespace pyfi::option

#endif //OPTION_H
