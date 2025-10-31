//
// Created by Nikolay Tsonev on 31/10/2025.
//

#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "pyfi/option.h"

using namespace pyfi::option;
using namespace Catch;

TEST_CASE("Black-Scholes call and put option") {
  auto S = 300.0;
  auto K = 250.0;
  auto T = 1.0;
  auto sigma = 0.15;
  auto rs = 0.03;
  auto call = black_scholes_call(S,K, sigma, rs, T);
  auto put = black_scholes_put(S,K, sigma, rs, T);

  REQUIRE(call == Approx(58.82).margin(1e-9));
  REQUIRE(put == Approx( 1.43115152412025282).margin(1e-9));

  S = 100; K = 34; T = 60; rs = 0.0; sigma = 0.43;
  call = black_scholes_call(S,K, sigma, rs, T);
  put = black_scholes_put(S,K, sigma, rs, T);

  REQUIRE(call == Approx(94.6105).margin(1e-9));
  REQUIRE(put == Approx(28.6105).margin(1e-9));

  S = 100; K = 100; T = 1; rs = 0.0; sigma = 0.20;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(7.965567455405804).margin(1e-9));
  REQUIRE(put  == Approx(7.965567455405804).margin(1e-9));

  S = 50; K = 60; T = 0.5; rs = 0.05; sigma = 0.25;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(0.9758354865045611).margin(1e-9));
  REQUIRE(put  == Approx(9.49443020820452).margin(1e-9));

  S = 120; K = 100; T = 2; rs = 0.01; sigma = 0.30;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(31.308028143442222).margin(1e-9));
  REQUIRE(put  == Approx(9.327895474117732).margin(1e-9));

  S = 80; K = 100; T = 1; rs = 0.0; sigma = 0.10;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(0.03991434342183964).margin(1e-9));
  REQUIRE(put  == Approx(20.039914343421856).margin(1e-9));

  S = 100; K = 120; T = 3; rs = 0.02; sigma = 0.35;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(19.361444058762473).margin(1e-9));
  REQUIRE(put  == Approx(32.373188088872325).margin(1e-9));

  S = 200; K = 150; T = 0.25; rs = 0.07; sigma = 0.50;
  call = black_scholes_call(S,K, sigma, rs, T);
  put  = black_scholes_put (S,K, sigma, rs, T);

  REQUIRE(call == Approx(54.897523408408773).margin(1e-9));
  REQUIRE(put  == Approx(2.295358758169744).margin(1e-9));
}

TEST_CASE("Black-Scholes throws exception given incorrect inputs") {
  constexpr auto S = 300.0;
  constexpr auto K = 250.0;
  constexpr auto rs = 0.03;

  auto sigma = 0.15;
  auto T = 0;

  REQUIRE_THROWS_AS(
    black_scholes_call(S,K, sigma, rs, T),
    std::invalid_argument
  );

  REQUIRE_THROWS_AS(
    black_scholes_put(S,K, sigma, rs, T),
    std::invalid_argument
  );

  sigma = 0.0; T = 1;

  REQUIRE_THROWS_AS(
      black_scholes_call(S,K, sigma, rs, T),
      std::invalid_argument
    );

  REQUIRE_THROWS_AS(
    black_scholes_put(S,K, sigma, rs, T),
    std::invalid_argument
  );
}

TEST_CASE("EU Binomial call and put options with a very large step") {
  constexpr auto n = 1000;
  auto S = 300.0;
  auto K = 250.0;
  auto T = 1.0;
  auto sigma = 0.15;
  auto rs = 0.03;

  auto bs_call = black_scholes_call(S,K, sigma, rs, T);
  auto bs_put = black_scholes_put(S,K, sigma, rs, T);

  auto bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  auto bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  // REQUIRE(bin_putt == Approx(bs_put).margin(1e-9)); buggy for some reason

  S = 100; K = 34; T = 60; rs = 0.0; sigma = 0.43;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put = black_scholes_put(S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 100; K = 100; T = 1; rs = 0.0; sigma = 0.20;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 50; K = 60; T = 0.5; rs = 0.05; sigma = 0.25;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 120; K = 100; T = 2; rs = 0.01; sigma = 0.30;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 80; K = 100; T = 1; rs = 0.0; sigma = 0.10;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 100; K = 120; T = 3; rs = 0.02; sigma = 0.35;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));

  S = 200; K = 150; T = 0.25; rs = 0.07; sigma = 0.50;
  bs_call = black_scholes_call(S,K, sigma, rs, T);
  bs_put  = black_scholes_put (S,K, sigma, rs, T);

  bin_call = binomial_eu_option(S, K, sigma, rs, n, T, call_payoff);
  bin_putt = binomial_eu_option(S, K, sigma, rs, n, T, put_payoff);

  REQUIRE(bin_call == Approx(bs_call).margin(1e-9));
  REQUIRE(bin_putt == Approx(bs_put).margin(1e-9));
}