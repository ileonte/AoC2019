#include <fmt/format.h>
#include <iostream>

static inline long fuel_requirements_simple(long mass) {
    return mass / 3 - 2;
}

static inline long fuel_requirements_complex(long mass) {
    long total{0};
    while (mass > 8) {
        long delta = fuel_requirements_simple(mass);
        total += delta;
        mass = delta;
    }
    return total;
}

int main() {
    long total_simple{0};
    long total_complex{0};
    long mass{0};

    std::cin >> mass;
    while (!std::cin.fail()) {
        total_simple += fuel_requirements_simple(mass);
        total_complex += fuel_requirements_complex(mass);
        std::cin >> mass;
    }
    fmt::print("{}\n", total_simple);
    fmt::print("{}\n", total_complex);
    return 0;
}
