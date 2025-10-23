//
// Created by Nikolay Tsonev on 23/10/2025.
//
#include <iostream>

#include "pyfi/Bond.h"


int main(void)
{
    constexpr int start = 500;
    constexpr int stop  = 1200;    // exclusive
    constexpr int scale = 120;
    constexpr size_t n = stop - start;

    std::vector<float> c(n);
    for (size_t i = 0; i < n; ++i)
        c[i] = scale * (start + static_cast<int>(i));
    auto myBond = pyfi::Bond<float>(c, 5.0, 1, 1);

    std::cout << myBond.pv() << std::endl;
}