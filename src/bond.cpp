//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include "../include/pyfi/bond.h"
#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <vector>

#include <boost/math/tools/roots.hpp>

namespace pyfi::bond {
    template <std::floating_point T>
    T present_value(const std::vector<T>& cash_flows,
        const T annual_yield,
        T par_value,
        const int years,
        int compounding_annually,
        const bool same_cashflows) {
        if (years < 0 || compounding_annually <= 0) {
            throw std::invalid_argument("bad tenor or m");
        }

        const int n = years * compounding_annually;
        const T r = annual_yield / static_cast<T>(compounding_annually);
        if (r <= T{-1}) {
            throw std::invalid_argument("rate <= -100%/period");
        }

        if (same_cashflows) {
            if (n <= 0 || cash_flows.empty())
                return T{0};
            const T c = cash_flows.front();
            const T one{1};
            const T dfN = std::pow(one + r, -static_cast<T>(n));
            const T annuity = (r == T{0}) ? static_cast<T>(n) : (one - dfN) / r;
            return c * annuity + par_value * dfN;
        }

        T pv{0};
        const T one{1};
        for (std::size_t k = 0; k < cash_flows.size(); ++k) {
            pv += cash_flows[k] * std::pow(one + r, -static_cast<T>(k + 1));
        }

        if (n > 0 && static_cast<int>(cash_flows.size()) < n) {
            pv += par_value * std::pow(one + r, -static_cast<T>(n));
        }

        return pv;
    }

    template <std::floating_point T>
    std::vector<T> build_bond_cashflows(T par_value, T coupon_rate, const int years, int m) {
        if (m <= 0 || years <= 0)
            return {};
        const int n = years * m;
        const T c = par_value * (coupon_rate / static_cast<T>(m));
        std::vector<T> cf;
        cf.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n - 1; ++i)
            cf.push_back(c);
        cf.push_back(c + par_value);
        return cf;
    }

    template <std::floating_point T>
    T internal_rate_return(const std::vector<T>& cash_flows,
        const T price,
        const T interest_rate,
        const T par_value,
        const int years,
        const int compounding_annually) {

        using boost::math::tools::bracket_and_solve_root;
        using boost::math::tools::eps_tolerance;
        using boost::math::tools::toms748_solve;


        const int m = compounding_annually > 0 ? compounding_annually : 1;

        std::vector<T> cf = cash_flows.empty() ? build_bond_cashflows(par_value, interest_rate, years, m) : cash_flows;

        if (cf.empty()) {
            throw std::invalid_argument("No cash flows");
        }

        if (!(price > T{0})) {
            throw std::invalid_argument("Price must be > 0");
        }

        auto f = [&](T rp) {
            if (rp <= T{-1})
                return std::numeric_limits<T>::infinity();
            T v = T{0};
            T df = T{1};
            const T inv = T{1} / (T{1} + rp);
            for (std::size_t k = 0; k < cf.size(); ++k) {
                df *= inv;
                v += cf[k] * df;
            }
            return v - price;
        };

        constexpr bool rising = false;
        T rp_guess = interest_rate / static_cast<T>(m);
        if (!std::isfinite(static_cast<double>(rp_guess)))
            rp_guess = T{0.05} / static_cast<T>(m);
        if (rp_guess <= T{-0.9})
            rp_guess = T{-0.5};

        const T min_rp = T{-1} + std::numeric_limits<T>::epsilon();
        const T max_rp = T{10};

        rp_guess = std::clamp(rp_guess, T{-1} + std::numeric_limits<T>::epsilon() * T{10}, T{10});

        std::uintmax_t it = 128;
        eps_tolerance<T> tol(std::numeric_limits<T>::digits - 6);

        auto bracket = bracket_and_solve_root(f, rp_guess, T{2}, rising, tol, it);

        auto root_bounds = toms748_solve(f, bracket.first, bracket.second, tol, it);
        const T rp = (root_bounds.first + root_bounds.second) / T{2};
        return std::pow(T{1} + rp, static_cast<T>(m)) - T{1};
    }

    template <std::floating_point T>
    T price_from_yield(const std::vector<T>& cash_flows, T yield, int m) {
        const T r = yield / static_cast<T>(m);
        T pv = 0;
        T disc = 1;
        for (size_t i = 0; i < cash_flows.size(); ++i) {
            disc *= (1 + r);
            pv += cash_flows[i] / disc;
        }
        return pv;
    }


    template double present_value<double>(const std::vector<double>&, double, double, int, int, bool);
    template float present_value<float>(const std::vector<float>&, float, float, int, int, bool);

    template double internal_rate_return<double>(const std::vector<double>& cash_flows,
        double price,
        double interest_rate,
        double par_value,
        int years,
        int compounding_annually);

    template std::vector<double> build_bond_cashflows(double par_value, double coupon_rate, int years, int m);

    template std::vector<float> build_bond_cashflows(float par_value, float coupon_rate, int years, int m);

    template float internal_rate_return<float>(const std::vector<float>& cash_flows,
        float price,
        float interest_rate,
        float par_value,
        int years,
        int compounding_annually);

    template double price_from_yield(const std::vector<double>& cash_flows, double yield, int m);
    template float price_from_yield(const std::vector<float>& cash_flows, float yield, int m);

    template long double
    present_value<long double>(const std::vector<long double>&, long double, long double, int, int, bool);

    template std::vector<long double> build_bond_cashflows<long double>(long double, long double, int, int);

    template long double
    internal_rate_return<long double>(const std::vector<long double>&, long double, long double, long double, int, int);

    template long double price_from_yield<long double>(const std::vector<long double>&, long double, int);


    template <std::floating_point T>
    T zero_coupon_price(T par_value, T annual_yield, T years_to_maturity, int m) {
        if (!(par_value > T{0}))          throw std::invalid_argument("par_value must be > 0");
        if (!(years_to_maturity >= T{0})) throw std::invalid_argument("years_to_maturity must be >= 0");
        if (!(m > 0))                     throw std::invalid_argument("m must be > 0");
        const int years_int = static_cast<int>(years_to_maturity);
        return zero_coupon_price_cexpr<T>(par_value, annual_yield, years_int, m);
    }

    template <std::floating_point T>
    T coupon_bond_price(T par_value, T coupon_rate, T annual_yield, T years_to_maturity, int m) {
        if (!(par_value > T{0}))          throw std::invalid_argument("par_value must be > 0");
        if (!(coupon_rate >= T{0}))       throw std::invalid_argument("coupon_rate must be >= 0");
        if (!(years_to_maturity >= T{0})) throw std::invalid_argument("years_to_maturity must be >= 0");
        if (!(m > 0))                     throw std::invalid_argument("m must be > 0");
        const int years_int = static_cast<int>(years_to_maturity);
        return coupon_bond_price_cexpr<T>(par_value, coupon_rate, annual_yield, years_int, m);
    }


    template float       zero_coupon_price<float>(float, float, float, int);
    template double      zero_coupon_price<double>(double, double, double, int);
    template long double zero_coupon_price<long double>(long double, long double, long double, int);

    template float       coupon_bond_price<float>(float, float, float, float, int);
    template double      coupon_bond_price<double>(double, double, double, double, int);
    template long double coupon_bond_price<long double>(long double, long double, long double, long double, int);

    // template <std::floating_point T>
    // T zero_coupon_price(T par_value,
    //                     T annual_yield,
    //                     T years_to_maturity,
    //                     int m)
    // {
    //     if (!(par_value > T{0}))          throw std::invalid_argument("par_value must be > 0");
    //     if (!(years_to_maturity >= T{0})) throw std::invalid_argument("years_to_maturity must be >= 0");
    //     if (!(m > 0))                     throw std::invalid_argument("m must be > 0");
    //     // negative annual_yield allowed

    //     const T n      = static_cast<T>(m) * years_to_maturity;
    //     const T per_r  = annual_yield / static_cast<T>(m);
    //     return par_value / std::pow(T{1} + per_r, n);
    // }

    // template <std::floating_point T>
    // T coupon_bond_price(T par_value,
    //                     T coupon_rate,
    //                     T annual_yield,
    //                     T years_to_maturity,
    //                     int m)
    // {
    //     if (!(par_value > T{0}))          throw std::invalid_argument("par_value must be > 0");
    //     if (!(coupon_rate >= T{0}))       throw std::invalid_argument("coupon_rate must be >= 0");
    //     if (!(years_to_maturity >= T{0})) throw std::invalid_argument("years_to_maturity must be >= 0");
    //     if (!(m > 0))                     throw std::invalid_argument("m must be > 0");

    //     const auto cfs = build_bond_cashflows<T>(par_value, coupon_rate, years_to_maturity, m);
    //     return price_from_yield<T>(cfs, annual_yield, m);
    // }

} // namespace pyfi::bond
