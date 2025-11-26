"""
Simple test file that calls all pyfi.option functions.
"""

from __future__ import annotations

from pyfi import option as opt


def main() -> None:
    print("=== Black-Scholes ===")

    x1 = opt.black_scholes_x(100.0, 100.0, 0.2, 0.05, 1.0, 0.0)
    print(f"black_scholes_x: {x1}")

    call = opt.black_scholes_call(100.0, 100.0, 0.2, 0.05, 1.0, 0.0)
    print(f"black_scholes_call: {call}")

    put = opt.black_scholes_put(100.0, 100.0, 0.2, 0.05, 1.0, 0.0)
    print(f"black_scholes_put: {put}")

    print("\n=== Normal PDF ===")

    pdf1 = opt.norm_pdf(0.0)
    print(f"norm_pdf(0.0): {pdf1}")

    pdf2 = opt.norm_pdf(100.0, 100.0, 0.2, 0.05, 1.0)
    print(f"norm_pdf(S, K, sigma, r, T): {pdf2}")

    print("\n=== Greeks ===")

    call_delta = opt.bs_call_delta(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_call_delta: {call_delta}")

    put_delta = opt.bs_put_delta(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_put_delta: {put_delta}")

    gamma = opt.bs_gamma(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_gamma: {gamma}")

    call_theta = opt.bs_call_theta(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_call_theta: {call_theta}")

    put_theta = opt.bs_put_theta(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_put_theta: {put_theta}")

    vega = opt.bs_vega(100.0, 110.0, 0.25, 0.03, 0.01, 0.5)
    print(f"bs_vega: {vega}")

    rho_calc = opt.bs_rho_calculation(100.0, 0.05, 1.0, 0.5)
    print(f"bs_rho_calculation: {rho_calc}")

    call_rho = opt.bs_call_rho(100.0, 110.0, 0.25, 0.03, 0.5)
    print(f"bs_call_rho: {call_rho}")

    put_rho = opt.bs_put_rho(100.0, 110.0, 0.25, 0.03, 0.5)
    print(f"bs_put_rho: {put_rho}")

    print("\n=== Binomial Tree ===")

    eu_call = opt.binomial_eu_option(100.0, 100.0, 0.2, 0.05, 200, 1.0, "call")
    print(f"binomial_eu_option (call): {eu_call}")

    eu_put = opt.binomial_eu_option(100.0, 100.0, 0.2, 0.05, 200, 1.0, "put")
    print(f"binomial_eu_option (put): {eu_put}")

    us_call = opt.binomial_us_option(100.0, 100.0, 0.2, 0.05, 200, 1.0, "call")
    print(f"binomial_us_option (call): {us_call}")

    us_put = opt.binomial_us_option(100.0, 100.0, 0.2, 0.05, 200, 1.0, "put")
    print(f"binomial_us_option (put): {us_put}")

    print("\n=== Forward and Yield ===")

    forward = opt.forward_from_yield(100.0, 0.04, 2.0, 0.01)
    print(f"forward_from_yield: {forward}")

    implied_yield = opt.yield_from_forward(100.0, forward, 0.04, 2.0)
    print(f"yield_from_forward: {implied_yield}")

    print("\n=== All functions called successfully ===")


if __name__ == "__main__":
    main()
