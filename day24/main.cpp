#include <aoc.h>

static inline void set_state(unsigned& storage, int x, int y, bool v) {
    auto shift = y * 5 + x;
    auto mask = (v ? 1 : 0) << shift;
    storage |= mask;
}

static inline int get_state(unsigned storage, int x, int y) {
    if ((std::clamp(x, 0, 4) != x) || (std::clamp(y, 0, 4) != y))
        return 0;
    auto shift = y * 5 + x;
    return (storage & (1 << shift)) >> shift;
}

[[maybe_unused]] static inline void print_state(unsigned storage) {
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++)
            fmt::print("{}", get_state(storage, x, y) ? '#' : '.');
        fmt::print("\n");
    }
    fmt::print("\n");
}

static inline unsigned read_state(std::istream& in = std::cin) {
    unsigned storage{0};
    std::string s{};
    for (int y = 0; y < 5; y++) {
        std::getline(in, s);
        if (s.size() != 5) {
            fmt::print("Line {}: expected {} chars, got {}\n", y + 1, 5, s.size());
            return -1;
        }
        for (int x = 0; x < 5; x++)
            set_state(storage, x, y, s.at(x) == '#');
    }
    return storage;
}

static inline unsigned evolve(unsigned state) {
    unsigned new_state{0};
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int sum = get_state(state, x - 1, y) + get_state(state, x + 1, y) +
                      get_state(state, x, y - 1) + get_state(state, x, y + 1);
            if (get_state(state, x, y))
                set_state(new_state, x, y, sum == 1);
            else
                set_state(new_state, x, y, std::clamp(sum, 1, 2) == sum);
        }
    }
    return new_state;
}

static inline void part1(unsigned state) {
    std::unordered_set<unsigned> states{};
    states.insert(state);

    while (true) {
        state = evolve(state);
        auto [it, inserted] = states.insert(state);
        if (!inserted)
            break;
    }

    int ret{0};
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 5; x++) {
            int shift = y * 5 + x;
            int mask = 1 << shift;
            if (state & mask)
                ret += mask;
        }
    }
    fmt::print("{}\n", ret);
    print_state(state);
}

int main() {
    auto state = read_state();
    auto c = state;
    for (int i = 0; i < 10; i++) {
        c = evolve(c);
    }
    print_state(c);

    part1(state);

    return 0;
}
