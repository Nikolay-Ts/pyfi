//
// Created by Nikolay Tsonev on 23/10/2025.
//

#ifndef BOND_H
#define BOND_H

#include <concepts>
#include <vector>

namespace pyfi::bond {

    /**
     * Present value of a stream of cash flows under discrete compounding.
     *
     * If `same_cashflows` is true, the function assumes a level coupon stream
     * (annuity) of size `cash_flows.front()` for `years * compounding_annually`
     * periods and adds the discounted `par_value` at maturity. Otherwise, it
     * discounts each element of `cash_flows` at successive periods and, if the
     * stream is shorter than `years * compounding_annually`, redeems `par_value`
     * at maturity.
     *
     * @param cash_flows vector of cash flows paid once per period (1..n)
     * @param annual_yield annual yield (discrete, not continuous)
     * @param par_value redemption amount paid at maturity if applicable
     * @param years integer tenor in years
     * @param compounding_annually periods per year (e.g., 1, 2, 4, 12)
     * @param same_cashflows treat stream as a level annuity if true
     * @return discounted present value
     * @throw std::invalid_argument if years < 0, compounding_annually <= 0,
     *        or per-period rate <= -100%
     */
    [[nodiscard]] double present_value(const std::vector<double>& cash_flows,
        double annual_yield,
        double par_value,
        int years,
        int compounding_annually,
        bool same_cashflows);

    /**
     * Internal rate of return (yield-to-maturity per year) for a bond cash-flow schedule.
     *
     * If `cash_flows` is empty, a coupon schedule is synthesized from
     * `par_value`, `interest_rate` (used as the coupon rate), `years` and `compounding_annually`.
     * The root is solved on the per-period rate and annualized as (1 + rp)^m - 1.
     *
     * @param cash_flows explicit cash flows per period; may be empty to synthesize
     * @param price clean price to match (present value target)
     * @param interest_rate coupon rate (used when building flows if `cash_flows` is empty)
     * @param par_value redemption amount
     * @param years integer tenor in years
     * @param compounding_annually periods per year (m)
     * @return annualized yield (discrete compounding)
     * @throw std::invalid_argument if price <= 0 or flows are unavailable
     */
    double internal_rate_return(const std::vector<double>& cash_flows,
        double price,
        double interest_rate,
        double par_value,
        int years,
        int compounding_annually);

    /**
     * Build a standard bond cash-flow vector (coupon-only periods, last period includes principal).
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate (e.g., 0.05 for 5%)
     * @param years integer tenor in years
     * @param m periods per year
     * @return vector of size years*m with coupons; last element adds par_value
     * @note returns empty vector if m <= 0 or years <= 0
     */
    std::vector<double> build_bond_cashflows(double par_value, 
        double coupon_rate,
        int years,
        int m);

    /**
     * Price from a yield and an arbitrary cash-flow vector under discrete compounding.
     *
     * @param cash_flows cash flows paid once per period (1..n)
     * @param yield annual yield (discrete)
     * @param m periods per year
     * @return present value
     */
    double price_from_yield(const std::vector<double>& cash_flows,
        double yield, 
        int m);

    /**
     * Zero-coupon bond price with fractional-maturity support (discrete compounding).
     *
     * Uses exponentiation with a real number of periods: n = years_to_maturity * m.
     *
     * @param par_value face value paid at maturity
     * @param annual_yield annual yield (discrete)
     * @param years_to_maturity time to maturity in years (can be fractional)
     * @param m periods per year (default 2)
     * @return present value of the zero-coupon bond
     * @throw std::invalid_argument if m <= 0
     */
    double zero_coupon_price(double par_value, 
        double annual_yield, 
        double years_to_maturity, 
        int m = 2);

    /**
     * Coupon bond price with fractional-maturity support (discrete compounding).
     *
     * Computes full coupon PVs for floor(T*m) periods and, if there is a stub
     * fraction alpha = T*m - floor(T*m), discounts a final cash flow of
     * (par_value + coupon * alpha) at time n + alpha using (1 + y/m)^(n+alpha).
     * This uses linear accrual for the stub coupon; swap in a day-count
     * convention if required by market rules.
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate (e.g., 0.05 for 5%)
     * @param annual_yield annual yield to maturity (discrete)
     * @param years_to_maturity time to maturity in years (can be fractional)
     * @param m periods per year (default 2)
     * @return present value of the coupon bond
     * @throw std::invalid_argument if m <= 0
     */
    double coupon_bond_price(double par_value, 
        double coupon_rate, 
        double annual_yield, 
        double years_to_maturity, 
        int m = 2);

    /**
     * Forward value under continuous compounding.
     *
     * @param current_price spot price today
     * @param annual_yield continuously compounded annual rate if interpreted literally
     * @param years_to_forward time in years to forward horizon
     * @return forward price as current_price * exp(annual_yield * years_to_forward)
     * @note If you prefer discrete compounding for consistency, use pow(1 + r, t) instead.
     */
    double forward_value(double current_price, 
        double annual_yield, 
        double years_to_forward);

    /**
     *
     * Accrued interest using linear accrual within the current coupon period.
     * AI = C * alpha, where C = par_value * (coupon_rate / m) and alpha ∈ [0,1).
     *
     * @param par_value face value of the bond
     * @param coupon_rate annual coupon rate (e.g., 0.05 for 5%)
     * @param m number of coupon payments per year (e.g., 2 = semiannual)
     * @param accrued_fraction alpha = fraction of the current period elapsed since the last coupon in [0,1)
     * @return accrued interest between last coupon and settlement
     * @throw std::invalid_argument if m <= 0 or accrued_fraction ∉ [0,1)
     */
    double accrued_interest(double par_value, 
        double coupon_rate, 
        int m, 
        double accrued_fraction);

    /**
     * Calculates the dirty price (includes accrued interest) of a bond.
     *
     * The dirty price discounts all future coupon and principal payments
     * from the settlement date (within the current coupon period).
     *
     * Dirty = Σ C / (1 + y/m)^(k - α) + P / (1 + y/m)^(n - α)
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate
     * @param annual_yield yield to maturity (discrete compounding)
     * @param periods_remaining number of coupon payments remaining
     * @param m coupon payments per year
     * @param accrued_fraction fraction of current period elapsed (α)
     * @return dirty price
     */
    double dirty_coupon_price(double par_value,
        double coupon_rate,
        double annual_yield,
        int periods_remaining,
        int m,
        double accrued_fraction);

    /**
     * Calculates the clean price (market quoted) of a bond.
     *
     * Clean = Dirty - Accrued.
     * On coupon dates (α = 0), clean and dirty prices are equal.
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate
     * @param annual_yield yield to maturity (discrete compounding)
     * @param periods_remaining number of coupon payments remaining
     * @param m coupon payments per year
     * @param accrued_fraction fraction of current period elapsed (α)
     * @return clean price
     */
    double clean_coupon_price(double par_value,
        double coupon_rate,
        double annual_yield,
        int periods_remaining,
        int m,
        double accrued_fraction);

    /**
     * Calculates the dirty price using time to maturity (T) in years.
     *
     * Internally derives:
     *   N = T * m
     *   n = ceil(N)
     *   α = 1 - (N - floor(N))
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate
     * @param annual_yield yield to maturity (discrete compounding)
     * @param years_to_maturity time to maturity in years (fractional allowed)
     * @param m coupon payments per year
     * @return dirty price
     */
    double dirty_coupon_price_from_T(double par_value,
                                    double coupon_rate,
                                    double annual_yield,
                                    double years_to_maturity,
                                    int m);

    /**
     * Calculates the clean price using time to maturity (T) in years.
     *
     * Same logic as dirty_coupon_price_from_T but subtracts accrued interest since its clean.
     *
     * Example: for T = 3.75 years and m = 2 → N = 7.5, n = 8, α = 0.5.
     *
     * @param par_value face value
     * @param coupon_rate annual coupon rate
     * @param annual_yield yield to maturity (discrete compounding)
     * @param years_to_maturity time to maturity in years (fractional allowed)
     * @param m coupon payments per year
     * @return clean price
     */
    double clean_coupon_price_from_T(double par_value,
                                    double coupon_rate,
                                    double annual_yield,
                                    double years_to_maturity,
                                    int m);

} // namespace pyfi::bond


#endif // BOND_H
