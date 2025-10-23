//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include "../include/pyfi/Bond.h"
#include <cmath>
#include <algorithm>


namespace pyfi {
    template <typename T> requires std::is_arithmetic_v<T>
    Bond<T>::Bond(const std::vector<T>& cf, T interest, const int time, const int compounding_annual) :
        cashflows_(cf),
        interest_(interest),
        time_(time),
        compounding_annual_(compounding_annual),
        pv_(0.0),
        same_cashflow_(false) { }

    template <typename T> requires std::is_arithmetic_v<T>
    float Bond<T>::pv() {
        if (pv > 0) { return pv; }

        if (same_cashflow_ && interest_ != 0) {
            auto top = (1 - pow(1 + interest_, -(cashflows_.size()))) / interest_ ;
            return cashflows_[0] * top;
        }

        same_cashflow_ =  std::adjacent_find(
            cashflows_.begin(),
            cashflows_.end(),
            std::not_equal_to<>() == cashflows.end()
        );

        for (int i = 0; i < cashflows_.size(); i++) {
            pv += cashflows_[i] * pow((1 + interest_), -(i+1));
        }

        return pv;
    }

} // namespace pyfi
