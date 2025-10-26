//
// Created by Nikolay Tsonev on 23/10/2025.
//

#ifndef BOND_H
#define BOND_H

#include <concepts>
#include <vector>

namespace pyfi::bond {
  template <std::floating_point T>
  [[nodiscard]] T present_value(
      const std::vector<T> &cash_flows,
      T annual_yield,
      T par_value,
      int years,
      int compounding_annually,
      bool same_cashflows
  );

  template <std::floating_point T>
  T internal_rate_return(
      const std::vector<T> &cash_flows,
      T price,
      T interest_rate,
      T par_value,
      int years,
      int compounding_annually
  );

  template <std::floating_point T>
  std::vector<T> build_bond_cashflows(
    T par_value,
    T coupon_rate,
    int years,
    int m
  );

  template <std::floating_point T>
  T price_from_yield(const std::vector<T> &cash_flows, T yield, int m);


} // namespace pyfi



#endif //BOND_H
