//
// Created by Nikolay Tsonev on 09/11/2025.
//

#include <iostream>
#include "pyfi/option.h"

using namespace pyfi::option;

int main(void) {
    const auto gamma = bs_put_theta(6, 6, 6, 6, 6, 6);

    std::cout << "The gamma is " << gamma << std::endl;
}
