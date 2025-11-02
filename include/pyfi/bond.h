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

    template <std::floating_point T>
    constexpr T zero_coupon_price_cexpr(T par_value, T annual_yield, int years, int m) noexcept {
        const int n = years * m;                
        const T per = annual_yield / static_cast<T>(m);
        const T base = T{1} + per;
        T acc = T{1};
        for (int i = 0; i < n; ++i) acc *= base;

        return par_value / acc;
    }

    template <std::floating_point T>
    constexpr T coupon_bond_price_cexpr(T par_value, T coupon_rate, T annual_yield, int years, int m) noexcept {
        const int n = years * m;
        const T per = annual_yield / static_cast<T>(m);
        const T base = T{1} + per;
        const T C = par_value * (coupon_rate / static_cast<T>(m));
        
        T pv = T{0};
        T disc = T{1};
        for (int k = 1; k <= n; ++k) {
            disc *= base;         
            pv += C / disc;
        }
        pv += par_value / disc;      
        return pv;
    }

    template <std::floating_point T>
    T zero_coupon_price(T par_value, T annual_yield, T years_to_maturity, int m = 2);

    template <std::floating_point T>
    T coupon_bond_price(T par_value, T coupon_rate, T annual_yield, T years_to_maturity, int m = 2);

} // namespace pyfi::bond


#endif // BOND_H
