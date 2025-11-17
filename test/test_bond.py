import pyfi
from pyfi import bond


def main() -> None:
    print("pyfi module:", pyfi)
    print("pyfi.bond submodule:", bond)

    # Common parameters
    par_value = 100.0
    coupon_rate = 0.05
    annual_yield = 0.04
    years = 5
    m = 2

    cash_flows = [5.0] * (years * m)
    pv = bond.present_value(
        cash_flows=cash_flows,
        annual_yield=annual_yield,
        par_value=par_value,
        years=years,
        compounding_annually=m,
        same_cashflows=False,
    )
    print("present_value:", pv)

    # irr = bond.internal_rate_return(
    #     cash_flows=cash_flows,
    #     price=pv,
    #     interest_rate=coupon_rate,
    #     par_value=par_value,
    #     years=years,
    #     compounding_annually=m,
    # )
    # print("internal_rate_return:", irr)

    built_flows = bond.build_bond_cashflows(
        par_value=par_value,
        coupon_rate=coupon_rate,
        years=years,
        m=m,
    )
    print("build_bond_cashflows (len, head):", len(built_flows), built_flows[:5])

    price_from_y = bond.price_from_yield(
        cash_flows=built_flows,
        annual_yield=annual_yield,
        m=m,
    )
    print("price_from_yield:", price_from_y)

    zc_price = bond.zero_coupon_price(
        par_value=par_value,
        annual_yield=annual_yield,
        years_to_maturity=3.5,
        m=m,
    )
    print("zero_coupon_price:", zc_price)

    cb_price = bond.coupon_bond_price(
        par_value=par_value,
        coupon_rate=coupon_rate,
        annual_yield=annual_yield,
        years_to_maturity=3.5,
        m=m,
    )
    print("coupon_bond_price:", cb_price)

    fwd = bond.forward_value(
        current_price=par_value,
        annual_yield=0.03,
        years_to_forward=2.0,
    )
    print("forward_value:", fwd)

    accrued = bond.accrued_interest(
        par_value=par_value,
        coupon_rate=coupon_rate,
        m=m,
        accrued_fraction=0.5,
    )
    print("accrued_interest:", accrued)

    dirty = bond.dirty_coupon_price(
        par_value=par_value,
        coupon_rate=coupon_rate,
        annual_yield=annual_yield,
        periods_remaining=years * m,
        m=m,
        accrued_fraction=0.5,
    )
    print("dirty_coupon_price:", dirty)

    clean = bond.clean_coupon_price(
        par_value=par_value,
        coupon_rate=coupon_rate,
        annual_yield=annual_yield,
        periods_remaining=years * m,
        m=m,
        accrued_fraction=0.5,
    )
    print("clean_coupon_price:", clean)

    # 11) dirty_coupon_price_from_T
    dirty_T = bond.dirty_coupon_price_from_T(
        par_value=par_value,
        coupon_rate=coupon_rate,
        annual_yield=annual_yield,
        years_to_maturity=3.75,
        m=m,
    )
    print("dirty_coupon_price_from_T:", dirty_T)

    clean_T = bond.clean_coupon_price_from_T(
        par_value=par_value,
        coupon_rate=coupon_rate,
        annual_yield=annual_yield,
        years_to_maturity=3.75,
        m=m,
    )
    print("clean_coupon_price_from_T:", clean_T)


if __name__ == "__main__":
    main()
