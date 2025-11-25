//
// Created by Nikolay Tsonev on 21/11/2025.
//

#include "bond_bind.h"
#include <pybind11/pybind11.h>
#include "../include/pyfi/bond.h"

namespace py = pybind11;

void add_bond_module(py::module_& m) {
    using namespace pyfi::bond;

    m.def("present_value",
        &present_value,
        py::arg("cash_flows"),
        py::arg("annual_yield"),
        py::arg("par_value"),
        py::arg_v("years", 1, "1"),
        py::arg_v("compounding_annually", 1, "1"),
        py::arg_v("same_cashflows", false, "False"),
        R"doc(
        present_value(
            cash_flows: Sequence[float],
            annual_yield: float,
            par_value: float,
            years: int = 1,
            compounding_annually: int = 1,
            same_cashflows: bool = False
        ) -> float

        Present value of a stream of cash flows under discrete compounding.

        If `same_cashflows` is true, the function assumes a level coupon stream
        (annuity) of size `cash_flows.front()` for `years * compounding_annually`
        periods and adds the discounted `par_value` at maturity. Otherwise, it
        discounts each element of `cash_flows` at successive periods and, if the
        stream is shorter than `years * compounding_annually`, redeems `par_value`
        at maturity.

        Parameters
        ----------
        cash_flows :
            Vector of cash flows paid once per period (1..n).
        annual_yield :
            Annual yield (discrete, not continuous).
        par_value :
            Redemption amount paid at maturity if applicable.
        years :
            Integer tenor in years.
        compounding_annually :
            Periods per year (e.g., 1, 2, 4, 12).
        same_cashflows :
            Treat stream as a level annuity if true.
        )doc");

    m.def("internal_rate_return",
        &internal_rate_return,
        py::arg("cash_flows"),
        py::arg("price"),
        py::arg("interest_rate"),
        py::arg("par_value"),
        py::arg_v("years", 1, "1"),
        py::arg_v("compounding_annually", 1, "1"),
        R"doc(
        internal_rate_return(
            cash_flows: Sequence[float],
            price: float,
            interest_rate: float,
            par_value: float,
            years: int = 1,
            compounding_annually: int = 1
        ) -> float

        Internal rate of return (yield-to-maturity per year) for a bond cash-flow schedule.

        If `cash_flows` is empty, a coupon schedule is synthesized from
        `par_value`, `interest_rate` (used as the coupon rate), `years` and `compounding_annually`.
        The root is solved on the per-period rate and annualized as (1 + rp)^m - 1.

        Parameters
        ----------
        cash_flows :
            Explicit cash flows per period; may be empty to synthesize.
        price :
            Clean price to match (present value target).
        interest_rate :
            Coupon rate (used when building flows if `cash_flows` is empty).
        par_value :
            Redemption amount.
        years :
            Integer tenor in years.
        compounding_annually :
            Periods per year (m).
        )doc");

    m.def("build_bond_cashflows",
        &build_bond_cashflows,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg_v("years", 1, "1"),
        py::arg_v("m", 1, "1"),
        R"doc(
        build_bond_cashflows(
            par_value: float,
            coupon_rate: float,
            years: int = 1,
            m: int = 1
        ) -> list[float]

        Build a standard bond cash-flow vector (coupon-only periods, last period includes principal).

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate (e.g., 0.05 for 5%).
        years :
            Integer tenor in years.
        m :
            Periods per year.

        Returns
        -------
        list[float]
            Vector of size years * m with coupons; last element adds par_value.
            Returns empty vector if m <= 0 or years <= 0.
        )doc");

    m.def("price_from_yield",
        &price_from_yield,
        py::arg("cash_flows"),
        py::arg("annual_yield"),
        py::arg_v("m", 1, "1"),
        R"doc(
        price_from_yield(
            cash_flows: Sequence[float],
            annual_yield: float,
            m: int = 1
        ) -> float

        Price from a yield and an arbitrary cash-flow vector under discrete compounding.

        Parameters
        ----------
        cash_flows :
            Cash flows paid once per period (1..n).
        annual_yield :
            Annual yield (discrete).
        m :
            Periods per year.
        )doc");

    m.def("zero_coupon_price",
        &zero_coupon_price,
        py::arg("par_value"),
        py::arg("annual_yield"),
        py::arg("years_to_maturity"),
        py::arg_v("m", 2, "2"),
        R"doc(
        zero_coupon_price(
            par_value: float,
            annual_yield: float,
            years_to_maturity: float,
            m: int = 2
        ) -> float

        Zero-coupon bond price with fractional-maturity support (discrete compounding).

        Uses exponentiation with a real number of periods: n = years_to_maturity * m.

        Parameters
        ----------
        par_value :
            Face value paid at maturity.
        annual_yield :
            Annual yield (discrete).
        years_to_maturity :
            Time to maturity in years (can be fractional).
        m :
            Periods per year.

        Raises
        ------
        ValueError
            If m <= 0.
        )doc");

    m.def("coupon_bond_price",
        &coupon_bond_price,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("annual_yield"),
        py::arg("years_to_maturity"),
        py::arg_v("m", 2, "2"),
        R"doc(
        coupon_bond_price(
            par_value: float,
            coupon_rate: float,
            annual_yield: float,
            years_to_maturity: float,
            m: int = 2
        ) -> float

        Coupon bond price with fractional-maturity support (discrete compounding).

        Computes full coupon PVs for floor(T*m) periods and, if there is a stub
        fraction alpha = T*m - floor(T*m), discounts a final cash flow of
        (par_value + coupon * alpha) at time n + alpha using (1 + y/m)^(n+alpha).
        This uses linear accrual for the stub coupon; swap in a day-count
        convention if required by market rules.

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate (e.g., 0.05 for 5%).
        annual_yield :
            Annual yield to maturity (discrete).
        years_to_maturity :
            Time to maturity in years (can be fractional).
        m :
            Periods per year.

        Raises
        ------
        ValueError
            If m <= 0.
        )doc");

    m.def("forward_value",
        &forward_value,
        py::arg("current_price"),
        py::arg("annual_yield"),
        py::arg("years_to_forward"),
        R"doc(
        forward_value(
            current_price: float,
            annual_yield: float,
            years_to_forward: float
        ) -> float

        Forward value under continuous compounding.

        Forward price is current_price * exp(annual_yield * years_to_forward).

        Parameters
        ----------
        current_price :
            Spot price today.
        annual_yield :
            Continuously compounded annual rate if interpreted literally.
        years_to_forward :
            Time in years to forward horizon.

        Notes
        -----
        If you prefer discrete compounding for consistency, use pow(1 + r, t) instead.
        )doc");

    // accrued_interest
    m.def("accrued_interest",
        &accrued_interest,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("m"),
        py::arg("accrued_fraction"),
        R"doc(
        accrued_interest(
            par_value: float,
            coupon_rate: float,
            m: int,
            accrued_fraction: float
        ) -> float

        Accrued interest using linear accrual within the current coupon period.
        AI = C * alpha, where C = par_value * (coupon_rate / m) and alpha ∈ [0,1).

        Parameters
        ----------
        par_value :
            Face value of the bond.
        coupon_rate :
            Annual coupon rate (e.g., 0.05 for 5%).
        m :
            Number of coupon payments per year (e.g., 2 = semiannual).
        accrued_fraction :
            alpha = fraction of the current period elapsed since the last coupon in [0,1).

        Raises
        ------
        ValueError
            If m <= 0 or accrued_fraction ∉ [0,1).
        )doc");

    m.def("dirty_coupon_price",
        &dirty_coupon_price,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("annual_yield"),
        py::arg("periods_remaining"),
        py::arg("m"),
        py::arg("accrued_fraction"),
        R"doc(
        dirty_coupon_price(
            par_value: float,
            coupon_rate: float,
            annual_yield: float,
            periods_remaining: int,
            m: int,
            accrued_fraction: float
        ) -> float

        Calculates the dirty price (includes accrued interest) of a bond.

        The dirty price discounts all future coupon and principal payments
        from the settlement date (within the current coupon period).

        Dirty = Σ C / (1 + y/m)^(k - α) + P / (1 + y/m)^(n - α)

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate.
        annual_yield :
            Yield to maturity (discrete compounding).
        periods_remaining :
            Number of coupon payments remaining.
        m :
            Coupon payments per year.
        accrued_fraction :
            Fraction of current period elapsed (α).
        )doc");

    m.def("clean_coupon_price",
        &clean_coupon_price,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("annual_yield"),
        py::arg("periods_remaining"),
        py::arg("m"),
        py::arg("accrued_fraction"),
        R"doc(
        clean_coupon_price(
            par_value: float,
            coupon_rate: float,
            annual_yield: float,
            periods_remaining: int,
            m: int,
            accrued_fraction: float
        ) -> float

        Calculates the clean price (market quoted) of a bond.

        Clean = Dirty - Accrued.
        On coupon dates (α = 0), clean and dirty prices are equal.

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate.
        annual_yield :
            Yield to maturity (discrete compounding).
        periods_remaining :
            Number of coupon payments remaining.
        m :
            Coupon payments per year.
        accrued_fraction :
            Fraction of current period elapsed (α).
        )doc");

    m.def("dirty_coupon_price_from_T",
        &dirty_coupon_price_from_T,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("annual_yield"),
        py::arg("years_to_maturity"),
        py::arg("m"),
        R"doc(
        dirty_coupon_price_from_T(
            par_value: float,
            coupon_rate: float,
            annual_yield: float,
            years_to_maturity: float,
            m: int
        ) -> float

        Calculates the dirty price using time to maturity (T) in years.

        Internally derives:
            N = T * m
            n = ceil(N)
            α = 1 - (N - floor(N))

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate.
        annual_yield :
            Yield to maturity (discrete compounding).
        years_to_maturity :
            Time to maturity in years (fractional allowed).
        m :
            Coupon payments per year.
        )doc");

    m.def("clean_coupon_price_from_T",
        &clean_coupon_price_from_T,
        py::arg("par_value"),
        py::arg("coupon_rate"),
        py::arg("annual_yield"),
        py::arg("years_to_maturity"),
        py::arg("m"),
        R"doc(
        clean_coupon_price_from_T(
            par_value: float,
            coupon_rate: float,
            annual_yield: float,
            years_to_maturity: float,
            m: int
        ) -> float

        Calculates the clean price using time to maturity (T) in years.

        Same logic as dirty_coupon_price_from_T but subtracts accrued interest since it's clean.

        Example: for T = 3.75 years and m = 2 → N = 7.5, n = 8, α = 0.5.

        Parameters
        ----------
        par_value :
            Face value.
        coupon_rate :
            Annual coupon rate.
        annual_yield :
            Yield to maturity (discrete compounding).
        years_to_maturity :
            Time to maturity in years (fractional allowed).
        m :
            Coupon payments per year.
        )doc");
}
