// clang++ -Wall -Werror -std=c++20 -o simplex_test main.cpp simplex.cpp
#include "simplex.hpp"
#include <cstdint>
#include <iostream>

// constexpr char const *const chars[] = {"█", "▓", "▒", "░", " "};

int main() {
    for (uint32_t y = 0; y < 32; ++y) {
        for (uint32_t x = 0; x < 64; ++x) {
            double amount = Simplex::generate(y / 5., x / 5., 0);
            // int which = (((amount + 1.) / 2.) * 5.);
            // std::cout << chars[which];
            std::cout << "\x1b[38;5;" << 232 + static_cast<int>((amount + 1.) / 2. * 24) << "m█";
        }
        std::cout << '\n';
    }
    std::cout << "\x1b[39m";
    return 0;
}