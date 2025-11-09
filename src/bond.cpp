//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include <pyfi/bond.h>
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

    double zero_coupon_price(double par_value, double annual_yield, double years_to_maturity, int m) {
        const double n = years_to_maturity * static_cast<double>(m);                
        const double per = annual_yield / static_cast<double>(m);
        return par_value / std::pow(1.0 + per, n);
    }

    double coupon_bond_price(double par_value, double coupon_rate, double annual_yield, double years_to_maturity, int m) {
        const double per = annual_yield / static_cast<double>(m);
        const double base = 1.0 + per;
        const double C = par_value * (coupon_rate / static_cast<double>(m));
        const double n = years_to_maturity * static_cast<double>(m);
        const int nFull = static_cast<int>(std::floor(n));
        const double frac = n - static_cast<double>(nFull);
        double pv = 0.0;
        double disc = 1.0;

        for (int k = 1; k <= nFull; ++k) {
            disc *= base;         
            pv += C / disc;
        }
        if (frac > 0.0) { 
            const double df_stub = std::pow(base,frac);
            pv += (par_value + C * frac) / (disc * df_stub);
        }
        else {
        pv += par_value / disc;
        }
        
        return pv;
    }

    double forward_value(double current_price, double annual_yield, double years_to_forward){
        const double forward_val = current_price * std::exp(annual_yield * years_to_forward);
        return forward_val;
    }

    double accrued_interest(double par_value, double coupon_rate, int m, double accrued_fraction){
        const double C = par_value * (coupon_rate / static_cast<double>(m));
        return C * accrued_fraction;
    }

    double dirty_coupon_price(double par_value,
                            double coupon_rate,
                            double annual_yield,
                            int periods_remaining,
                            int m,
                            double accrued_fraction){
                                    const double r    = annual_yield / static_cast<double>(m);
                                    const double b    = 1.0 + r;
                                    const double C    = par_value * (coupon_rate / static_cast<double>(m));
                                    const int    n    = periods_remaining;

                                    if (std::abs(r) < 1e-15) {
                                        return C * static_cast<double>(n) + par_value;
                                    }
                                    const double b_neg_n = std::pow(b, -static_cast<double>(n));
                                    const double ann     = (1.0 - b_neg_n) / (b - 1.0);
                                    const double b_alpha = std::pow(b, accrued_fraction);

                                    return b_alpha * (C * ann + par_value * b_neg_n);
                                }
    
    double clean_coupon_price(double par_value,
                            double coupon_rate,
                            double annual_yield,
                            int periods_remaining,
                            int m,
                            double accrued_fraction) {
                                const double dirty = dirty_coupon_price(par_value, coupon_rate, annual_yield, periods_remaining, m, accrued_fraction);
                                const double ai = accrued_interest(par_value, coupon_rate, m, accrued_fraction);
                                return dirty - ai;
                            }

    double dirty_coupon_price_from_T(double par_value,
                                    double coupon_rate,
                                    double annual_yield,
                                    double years_to_maturity,
                                    int m) {
        const double N = years_to_maturity * static_cast<double>(m);
        const double fracN = N - std::floor(N);
        const int n = static_cast<int>(std::ceil(N));
        const double alpha = (std::abs(fracN) < 1e-12) ? 0.0 : (1.0 - fracN);

        return dirty_coupon_price(par_value, coupon_rate, annual_yield, n, m, alpha);
    }

    double clean_coupon_price_from_T(double par_value,
                                    double coupon_rate,
                                    double annual_yield,
                                    double years_to_maturity,
                                    int m) {
        const double N = years_to_maturity * static_cast<double>(m);
        const double fracN = N - std::floor(N);
        const int n = static_cast<int>(std::ceil(N));
        const double alpha = (std::abs(fracN) < 1e-12) ? 0.0 : (1.0 - fracN);

        return clean_coupon_price(par_value, coupon_rate, annual_yield, n, m, alpha);
    }

} // namespace pyfi::bond
