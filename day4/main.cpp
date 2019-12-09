#include <aoc.h>

using storage_type_t = unsigned;

template <int digit>
static inline unsigned get_digit(storage_type_t number_storage) {
    static_assert(std::clamp(digit, 0, 7) == digit);
    constexpr static storage_type_t shift = 4 * (7 - digit);
    constexpr static storage_type_t mask = 0x0f;
    return (number_storage >> shift) & mask;
}

template <int digit>
static inline void set_digit(storage_type_t& number_storage, unsigned value) {
    static_assert(std::clamp(digit, 0, 7) == digit);
    constexpr static storage_type_t shift = 4 * (7 - digit);
    constexpr static storage_type_t mask = 0x0f;
    number_storage |= storage_type_t(value & mask) << shift;
}

static inline storage_type_t int_to_storage(unsigned value) {
    storage_type_t ret{0};
    set_digit<7>(ret, value % 10); value /= 10;
    set_digit<6>(ret, value % 10); value /= 10;
    set_digit<5>(ret, value % 10); value /= 10;
    set_digit<4>(ret, value % 10); value /= 10;
    set_digit<3>(ret, value % 10); value /= 10;
    set_digit<2>(ret, value % 10); value /= 10;
    set_digit<1>(ret, value % 10); value /= 10;
    return ret;
}

static inline unsigned check_part1(unsigned number) {
    auto s = int_to_storage(number);
    if (get_digit<1>(s) > get_digit<2>(s)
        || get_digit<2>(s) > get_digit<3>(s)
        || get_digit<3>(s) > get_digit<4>(s)
        || get_digit<4>(s) > get_digit<5>(s)
        || get_digit<5>(s) > get_digit<6>(s)) return 0;
    return unsigned(
        get_digit<1>(s) == get_digit<2>(s) ||
        get_digit<2>(s) == get_digit<3>(s) ||
        get_digit<3>(s) == get_digit<4>(s) ||
        get_digit<4>(s) == get_digit<5>(s) ||
        get_digit<5>(s) == get_digit<6>(s)
    );
}

int main() {
    constexpr const unsigned lower_bound = 123257;
    constexpr const unsigned upper_bound = 647015;
    unsigned answer_part1{0};
    for (auto current_number = lower_bound; current_number <= upper_bound; current_number++) {
        auto v = check_part1(current_number);
        if (v) {
            answer_part1 += 1;
        }
    }
    fmt::print("{}\n", answer_part1);
    return 0;
}
