#include <aoc.h>

enum shuffle_type {
    deal_into_new_stack,
    cut,
    deal_with_increment,
};
static inline std::ostream& operator<<(std::ostream& out, shuffle_type t) {
    switch (t) {
        case deal_into_new_stack: {
            out << "deal into new stack";
            break;
        }
        case cut: {
            out << "cut";
            break;
        }
        case deal_with_increment: {
            out << "deal with increment";
            break;
        }
    }
    return out;
}

struct shuffle_action {
    shuffle_type type;
    int64_t N;

    inline int64_t apply(int64_t deck_size, int64_t current_position) const {
        assert(deck_size > 2);
        assert(std::clamp<int64_t>(current_position, 0, deck_size) == current_position);
        switch (type) {
            case deal_into_new_stack:
                return deck_size - current_position - 1;
            case cut: {
                int actual_cut = N > 0 ? N : deck_size + N;
                if (current_position < actual_cut)
                    return deck_size - actual_cut + current_position;
                return current_position - actual_cut;
            }
            case deal_with_increment:
                return (current_position * N) % deck_size;
            default:
                std::abort();
        }
    }

    static inline std::optional<shuffle_action> from_string(std::string_view sv) {
        static constexpr const std::string_view cut_sv("cut ");
        static constexpr const std::string_view deal_sv("deal with increment ");

        sv = aoc::trim(sv);
        if (sv == "deal into new stack") return shuffle_action{deal_into_new_stack, 0};

        if (aoc::substr(sv, 0, cut_sv.size()) == cut_sv) {
            shuffle_action act{cut, 0};
            auto N_sv = aoc::substr(sv, cut_sv.size());
            auto r = std::from_chars(N_sv.begin(), N_sv.end(), act.N);
            if (r.ec != std::errc{}) return {};
            return act;
        }

        if (aoc::substr(sv, 0, deal_sv.size()) == deal_sv) {
            shuffle_action act{deal_with_increment, 0};
            auto N_sv = aoc::substr(sv, deal_sv.size());
            auto r = std::from_chars(N_sv.begin(), N_sv.end(), act.N);
            if (r.ec != std::errc{}) return {};
            return act;
        }

        return {};
    }

    inline friend std::ostream& operator<<(std::ostream& out, shuffle_action act) {
        if (act.type != deal_into_new_stack)
            return out << "{" << act.type << ": " << act.N << "}";
        return out << "{" << act.type << "}";
    }
};

static inline std::vector<shuffle_action> read_input(std::istream& in = std::cin) {
    std::string line{};
    std::vector<shuffle_action> ret{};

    while (std::getline(in, line)) {
        auto ma = shuffle_action::from_string(line);
        if (!ma) {
            fmt::print(::stderr, "Invalid input line: '{}'\n", line);
            std::abort();
        }
        ret.push_back(ma.value());
    }
    return ret;
}

static inline int64_t apply_shuffle_actions(const std::vector<shuffle_action>& actions, int64_t deck_size, int64_t pos, int64_t count = 1) {
    for (auto i = 0; i < count; i++) {
        for (const auto& act : actions)
            pos = act.apply(deck_size, pos);
    }
    return pos;
}

static inline void part1(const std::vector<shuffle_action> actions) {
    fmt::print("{}\n", apply_shuffle_actions(actions, 10007, 2019));
}

int main() {
    if constexpr (DEBUG) {
        fmt::print("{}\n", shuffle_action{deal_into_new_stack, 0}.apply(10, 0));
        fmt::print("{}\n", shuffle_action{deal_into_new_stack, 0}.apply(10, 6));

        fmt::print("{}\n", shuffle_action{cut, 3}.apply(10, 0));
        fmt::print("{}\n", shuffle_action{cut, 3}.apply(10, 3));
        fmt::print("{}\n", shuffle_action{cut, 3}.apply(10, 4));

        fmt::print("{}\n", shuffle_action{cut, -4}.apply(10, 0));
        fmt::print("{}\n", shuffle_action{cut, -4}.apply(10, 1));
        fmt::print("{}\n", shuffle_action{cut, -4}.apply(10, 6));
        fmt::print("{}\n", shuffle_action{cut, -4}.apply(10, 7));

        fmt::print("{}\n", shuffle_action{deal_with_increment, 3}.apply(10, 0));
        fmt::print("{}\n", shuffle_action{deal_with_increment, 3}.apply(10, 1));
        fmt::print("{}\n", shuffle_action{deal_with_increment, 3}.apply(10, 4));
        fmt::print("{}\n", shuffle_action{deal_with_increment, 3}.apply(10, 5));
    }

    auto actions = read_input();

    part1(actions);

    fmt::print("{}\n{}\n", 119315717514047, std::numeric_limits<int64_t>::max());

    return 0;
}
