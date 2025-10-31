//
// Created by Nikolay Tsonev on 26/10/2025.
//

#ifndef OPTION_H
#define OPTION_H

#include <functional>
#include <vector>

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

  /**
   *
   *  This is the payoff function which shows if the option holder will
   *  exercise their call option right. This is given by
   * max(spot_rates-strike_price, 0) These functions are as helpers to the user,
   * but the user can implement their own payoff function.
   *
   * @param spot_rates
   * @param strike_price
   * @return spot_rates - strike_price if the user will exercise their by option
   */
  std::vector<double> call_payoff(const std::vector<double>& spot_rates, double strike_price);


  /**
   *
   *  This is the payoff function which shows if the option holder will
   *  exercise their put option right. This is given by max(strike_price-spot_rates, 0).
   *  These functions are as helpers to the user, but the user can implement their
   *  own payoff function.
   *
   * @param spot_rates
   * @param strike_price
   * @return strike_price - spot_rates if the user will exercise their by option
   */
  std::vector<double> put_payoff(const std::vector<double>& spot_rates, double strike_price);


  double binomial_eu_option(
    double stock_price,
    double strike_price,
    double volatility,
    double risk_free_rate,
    double time,
    std::function<std::vector<double>(const std::vector<double>&, double)>& payoff
  );

} // namespace pyfi::option

#endif //OPTION_H
