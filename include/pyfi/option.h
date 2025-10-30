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
   * @param volotility
   * @param risk_free_rate
   * @param time
   * @return price of the European call option
   */
  double black_scholes_price(
      double stock_price,
      double strike_price,
      double volotility,
      double risk_free_rate,
      uint64_t time
  );

} // namespace pyfi::option

#endif //OPTION_H
