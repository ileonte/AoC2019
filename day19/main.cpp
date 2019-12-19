#include <computer.h>

using value_type = aoc::computer::memory_value_t;

static value_type s_50_x{0};
static value_type e_50_x{0};

static inline auto value_at(const aoc::computer& c, size_t x, size_t y) {
    aoc::computer cc(c);
    cc.add_input({x, y});
    cc.expand_memory(1024);
    cc.execute();
    return cc.outputs().back();
};

static inline auto line_width(const aoc::computer& c, value_type line, value_type &start_x, value_type& end_x) {
    static std::unordered_map<value_type, std::pair<value_type, value_type>> known_lengths{};
    if (auto it = known_lengths.find(line); it != known_lengths.end()) {
        start_x = it->second.second;
        return it->second.first;
    }

    value_type ret{0};

    if (!start_x) {
        if (line > 50) {
            fmt::print(::stderr, "NOPE!");
            std::abort();
        }
        for (value_type x = 0; x < 50; x++) {
            if (value_at(c, x, line)) {
                start_x = x;
                break;
            }
        }
        if (!start_x && !value_at(c, start_x, line))
            return ret;
        end_x = start_x + 1;
        while (value_at(c, end_x, line)) end_x++;
    } else {
        while (!value_at(c, start_x, line))
            start_x++;
        while (value_at(c, end_x, line))
            end_x++;
    }

    ret = end_x - start_x;
    known_lengths[line] = std::make_pair(ret, start_x);

    return ret;
}

static inline void part1(const aoc::computer& c_in) {
    value_type ret{0};
    for (aoc::computer::memory_value_t y = 0; y < 50; y++) {
        auto w = line_width(c_in, y, s_50_x, e_50_x);
        ret += w;
    }
    fmt::print("{}\n", ret);
}

static inline void part2(const aoc::computer& c) {
    value_type top_s_x{s_50_x};
    value_type top_e_x{e_50_x};
    value_type top_y{51};

    auto width = line_width(c, top_y, top_s_x, top_e_x);
    while (width < 100) {
        top_y += 1;
        width = line_width(c, top_y, top_s_x, top_e_x);
    }

    auto bot_x = top_s_x + width - 100;
    auto bot_y = top_y + 100 - 1;

    while (!value_at(c, bot_x, bot_y)) {
        top_y += 1;
        width = line_width(c, top_y, top_s_x, top_e_x);
        bot_x = top_s_x + width - 100;
        bot_y = top_y + 100 - 1;
    }

    fmt::print("{}\n", bot_x * 10000 + top_y);
}

int main() {
    auto c = aoc::computer::read_initial_state();

    part1(c);
    part2(c);

    return 0;
}
