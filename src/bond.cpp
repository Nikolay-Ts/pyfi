//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include "../include/pyfi/bond.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include <boost/math/tools/roots.hpp>

namespace pyfi::bond {
    double present_value(const std::vector<double>& cash_flows,
        const double annual_yield,
        double par_value,
        const int years,
        int compounding_annually,
        const bool same_cashflows) {
        if (years < 0 || compounding_annually <= 0) {
            throw std::invalid_argument("bad tenor or m");
        }

        const int n = years * compounding_annually;
        const double r = annual_yield / static_cast<double>(compounding_annually);
        if (r <= double{-1}) {
            throw std::invalid_argument("rate <= -100%/period");
        }

        if (same_cashflows) {
            if (n <= 0 || cash_flows.empty())
                return double{0};
            const double c = cash_flows.front();
            const double one{1};
            const double dfN = std::pow(one + r, -static_cast<double>(n));
            const double annuity = (r == double{0}) ? static_cast<double>(n) : (one - dfN) / r;
            return c * annuity + par_value * dfN;
        }

        double pv{0};
        const double one{1};
        for (std::size_t k = 0; k < cash_flows.size(); ++k) {
            pv += cash_flows[k] * std::pow(one + r, -static_cast<double>(k + 1));
        }

        if (n > 0 && static_cast<int>(cash_flows.size()) < n) {
            pv += par_value * std::pow(one + r, -static_cast<double>(n));
        }

        return pv;
    }

    std::vector<double> build_bond_cashflows(double par_value, double coupon_rate, const int years, int m) {
        if (m <= 0 || years <= 0)
            return {};
        const int n = years * m;
        const double c = par_value * (coupon_rate / static_cast<double>(m));
        std::vector<double> cf;
        cf.reserve(static_cast<std::size_t>(n));
        for (int i = 0; i < n - 1; ++i)
            cf.push_back(c);
        cf.push_back(c + par_value);
        return cf;
    }

    double internal_rate_return(const std::vector<double>& cash_flows,
        const double price,
        const double interest_rate,
        const double par_value,
        const int years,
        const int compounding_annually) {

        using boost::math::tools::bracket_and_solve_root;
        using boost::math::tools::eps_tolerance;
        using boost::math::tools::toms748_solve;


        const int m = compounding_annually > 0 ? compounding_annually : 1;

        std::vector<double> cf = cash_flows.empty() ? build_bond_cashflows(par_value, interest_rate, years, m) : cash_flows;

        if (cf.empty()) {
            throw std::invalid_argument("No cash flows");
        }

        if (!(price > double{0})) {
            throw std::invalid_argument("Price must be > 0");
        }

        auto f = [&](double rp) {
            if (rp <= double{-1})
                return std::numeric_limits<double>::infinity();
            double v = double{0};
            double df = double{1};
            const double inv = double{1} / (double{1} + rp);
            for (std::size_t k = 0; k < cf.size(); ++k) {
                df *= inv;
                v += cf[k] * df;
            }
            return v - price;
        };

        constexpr bool rising = false;
        double rp_guess = interest_rate / static_cast<double>(m);
        if (!std::isfinite(static_cast<double>(rp_guess)))
            rp_guess = double{0.05} / static_cast<double>(m);
        if (rp_guess <= double{-0.9})
            rp_guess = double{-0.5};

        const double min_rp = double{-1} + std::numeric_limits<double>::epsilon();
        const double max_rp = double{10};

        rp_guess = std::clamp(rp_guess, double{-1} + std::numeric_limits<double>::epsilon() * double{10}, double{10});

        std::uintmax_t it = 128;
        eps_tolerance<double> tol(std::numeric_limits<double>::digits - 6);

        auto bracket = bracket_and_solve_root(f, rp_guess, double{2}, rising, tol, it);

        auto root_bounds = toms748_solve(f, bracket.first, bracket.second, tol, it);
        const double rp = (root_bounds.first + root_bounds.second) / double{2};
        return std::pow(double{1} + rp, static_cast<double>(m)) - double{1};
    }

    double price_from_yield(const std::vector<double>& cash_flows, double yield, int m) {
        const double r = yield / static_cast<double>(m);
        double pv = 0;
        double disc = 1;
        for (size_t i = 0; i < cash_flows.size(); ++i) {
            disc *= (1 + r);
            pv += cash_flows[i] / disc;
        }
        return pv;
    }

    static inline void validate_common(double par_value, double years_to_maturity, int m)
    {
        if (!(par_value > 0.0))          throw std::invalid_argument("par_value must be > 0");
        if (!(years_to_maturity >= 0.0)) throw std::invalid_argument("years_to_maturity must be >= 0");
        if (!(m > 0))                     throw std::invalid_argument("m must be > 0");
    }

    double zero_coupon_price(double par_value, double annual_yield, double years_to_maturity, int m) {
        validate_common(par_value, years_to_maturity, m);
        const int years_int = static_cast<int>(years_to_maturity);
        return zero_coupon_price_cexpr(par_value, annual_yield, years_int, m);
    }

    double coupon_bond_price(double par_value, double coupon_rate, double annual_yield, double years_to_maturity, int m) {
        validate_common(par_value, years_to_maturity, m);
        const int years_int = static_cast<int>(years_to_maturity);
        return coupon_bond_price_cexpr(par_value, coupon_rate, annual_yield, years_int, m);
    }

    double forward_value(double current_price, double annual_yield, double years_to_forward)noexcept{
        const double forward_val = current_price * std::exp(annual_yield * years_to_forward);
        return forward_val;
    }

} // namespace pyfi::bond
