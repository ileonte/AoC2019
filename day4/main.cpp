#include <aoc.h>

using storage_type_t = unsigned;

template <int digit>
static constexpr inline unsigned get_digit(storage_type_t number_storage) {
    static_assert(std::clamp(digit, 0, 7) == digit);
    constexpr storage_type_t shift = 4 * (7 - digit);
    constexpr storage_type_t mask = 0x0f;
    return (number_storage >> shift) & mask;
}

template <int digit>
static inline constexpr void set_digit(storage_type_t& number_storage, unsigned value) {
    static_assert(std::clamp(digit, 0, 7) == digit);
    constexpr storage_type_t shift = 4 * (7 - digit);
    constexpr storage_type_t mask = 0x0f;
    number_storage |= storage_type_t(value & mask) << shift;
}

template <int d1, int d2>
static inline constexpr bool digits_equal(storage_type_t s) {
    return get_digit<d1>(s) == get_digit<d2>(s);
}

template <int d>
static inline constexpr bool digit_cluster(storage_type_t s) {
    static_assert(std::clamp(d, 1, 5) == d);
    return
        (get_digit<d - 1>(s) != get_digit<d>(s)) &&
        (get_digit<d>(s) == get_digit<d + 1>(s)) &&
        (get_digit<d + 1>(s) != get_digit<d + 2>(s));
}

static constexpr inline storage_type_t int_to_storage(unsigned value) {
    storage_type_t ret{0};
    set_digit<6>(ret, value % 10); value /= 10;
    set_digit<5>(ret, value % 10); value /= 10;
    set_digit<4>(ret, value % 10); value /= 10;
    set_digit<3>(ret, value % 10); value /= 10;
    set_digit<2>(ret, value % 10); value /= 10;
    set_digit<1>(ret, value % 10); value /= 10;
    return ret;
}

static inline bool check_precondition(storage_type_t s) {
    return get_digit<1>(s) <= get_digit<2>(s)
           && get_digit<2>(s) <= get_digit<3>(s)
           && get_digit<3>(s) <= get_digit<4>(s)
           && get_digit<4>(s) <= get_digit<5>(s)
           && get_digit<5>(s) <= get_digit<6>(s);
}

static inline unsigned check_part1(storage_type_t s) {
    return digits_equal<1, 2>(s) || digits_equal<2, 3>(s) || digits_equal<3, 4>(s) || digits_equal<4, 5>(s) || digits_equal<5, 6>(s);
}

static inline unsigned check_part2(storage_type_t s) {
    return digit_cluster<1>(s) || digit_cluster<2>(s) || digit_cluster<3>(s) || digit_cluster<4>(s) || digit_cluster<5>(s);
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
static inline bool get_number(std::string_view src, T& dst) {
    auto sv = aoc::trim(src);
    auto r = std::from_chars(std::cbegin(sv), std::cend(sv), dst);
    if (r.ec != std::errc()) return false;
    return true;
}

int main() {
    unsigned lower_bound{};
    unsigned upper_bound{};
    unsigned answer_part1{0};
    unsigned answer_part2{0};
    std::string in{};

    std::getline(std::cin, in);
    const auto parts = aoc::str_split(in, '-');
    if (parts.size() != 2) {
        fmt::print("Invalid input: '{}'\n", in);
        return 1;
    }
    if (!get_number(parts[0], lower_bound) || !get_number(parts[1], upper_bound)) {
        fmt::print("Invalid input: '{}'\n", in);
        return 2;
    }

    for (auto current_number = lower_bound; current_number <= upper_bound; current_number++) {
        auto st = int_to_storage(current_number);
        if (!check_precondition(st)) continue;
        answer_part1 += check_part1(st);
        answer_part2 += check_part2(st);
    }
    fmt::print("{}\n{}\n", answer_part1, answer_part2);
    return 0;
}
