//
// Created by Nikolay Tsonev on 23/10/2025.
//

#ifndef BOND_H
#define BOND_H

#include <concepts>
#include <vector>

namespace pyfi::bond {
    template <std::floating_point T>
    [[nodiscard]] T present_value(const std::vector<T>& cash_flows,
        T annual_yield,
        T par_value,
        int years,
        int compounding_annually,
        bool same_cashflows);

    template <std::floating_point T>
    T internal_rate_return(const std::vector<T>& cash_flows,
        T price,
        T interest_rate,
        T par_value,
        int years,
        int compounding_annually);

    template <std::floating_point T>
    std::vector<T> build_bond_cashflows(T par_value, T coupon_rate, int years, int m);

    template <std::floating_point T>
    T price_from_yield(const std::vector<T>& cash_flows, T yield, int m);

    constexpr double zero_coupon_price_cexpr(double par_value, double annual_yield, int years, int m) noexcept {
        //m: compunding periods per year
        const int n = years * m;                
        const double per = annual_yield / static_cast<double>(m);
        const double base = 1.0 + per;
        double acc = 1.0;
        for (int i = 0; i < n; ++i) acc *= base;
        return par_value / acc;
    }

    constexpr double coupon_bond_price_cexpr(double par_value, double coupon_rate, double annual_yield, int years, int m) noexcept {
        //m: compounding periods per year
        const int n = years * m;
        const double per = annual_yield / static_cast<double>(m);
        const double base = 1.0 + per;
        const double C = par_value * (coupon_rate / static_cast<double>(m));
        double pv = 0.0;
        double disc = 1.0;
        for (int k = 1; k <= n; ++k) {
            disc *= base;         
            pv += C / disc;
            }
        pv += par_value / disc;      
        return pv;
    }

    double zero_coupon_price(double par_value, double annual_yield, double years_to_maturity, int m = 2);

    double coupon_bond_price(double par_value, double coupon_rate, double annual_yield, double years_to_maturity, int m = 2);

} // namespace pyfi::bond


#endif // BOND_H
