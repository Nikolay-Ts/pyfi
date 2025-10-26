//
// Created by Nikolay Tsonev on 23/10/2025.
//

#ifndef BOND_H
#define BOND_H

#include <concepts>
#include <vector>

namespace pyfi {

    template <std::floating_point T>
    class Bond {
    public:

        explicit Bond(const std::vector<T>& cf, T interest, int time, int compounding_annual);
        Bond(const Bond& b) = default;
        Bond& operator=(const Bond& b) = default;
        ~Bond() = default;

        T interest() const { return interest_; }
        int time() const   { return time_; }
        int compounding_annual() const { return compounding_annual_; }
        std::vector<T> cashflows() { return cashflows_; }

        /*
        * @brief calculates the present value of the bond
        *
        * @return the present value of the bond
        */
        T pv();

        /*
         * @brief calculates the present value of the bond
         *
         * @param same_cashflow if the user knows that the cashflows are the same
         * @return the present value of the bond
         */
         T pv(bool same_cashflow);

    private:
        std::vector<T> cashflows_;
        T interest_;
        T price_;
        int time_;
        int compounding_annual_;
        int pv_;
        bool same_cashflow_;
    };
} // namespace pyfi



#endif //BOND_H
