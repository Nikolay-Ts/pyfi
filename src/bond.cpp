//
// Created by Nikolay Tsonev on 23/10/2025.
//

#include "../include/pyfi/Bond.h"
#include <cmath>
#include <algorithm>

namespace pyfi {
    template <std::floating_point T>
    Bond<T>::Bond(const std::vector<T>& cf, T interest, const int time, const int compounding_annual) :
        cashflows_(cf),
        interest_(interest),
        time_(time),
        compounding_annual_(compounding_annual),
        pv_(0.0),
        same_cashflow_(false) { }

    template <std::floating_point T>
    T Bond<T>::pv() {
        if (pv_ > 0) { return pv_; }

        if (same_cashflow_ && interest_ != 0) {
            auto top = (1 - pow(1 + interest_, -(cashflows_.size()))) / interest_ ;
            return cashflows_[0] * top;
        }

        // same_cashflow_ =  std::adjacent_find(
        //     cashflows_.begin(),
        //     cashflows_.end(),
        //     std::not_equal_to<T>() == cashflows_.end()
        // );

        for (int i = 0; i < cashflows_.size(); i++)
            pv_ += cashflows_[i] * std::pow(1 + interest_, -static_cast<T>(i + 1));


        return pv_;
    }

    template <std::floating_point T>
    T Bond<T>::pv(const bool same_cashflow) {
        if (interest_ < 1e-9) {
            return pv();
        }

        if (same_cashflow) {
            auto top = (1 - pow(1 + interest_, -(cashflows_.size()))) / interest_ ;
            return cashflows_[0] * top;
        }

        return pv();
    }


    template class Bond<float>;
    template class Bond<double>;
} // namespace pyfi
