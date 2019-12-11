#include <computer.h>

struct point {
    int x;
    int y;
};
[[maybe_unused]] static inline bool operator==(point p1, point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}
[[maybe_unused]] static inline bool operator!=(point p1, point p2) {
    return !(p1 == p2);
}
namespace std {
    template <> struct hash<point> {
        size_t operator()(point const& p) const {
            if constexpr (sizeof(size_t) >= sizeof(p.x) + sizeof(p.y)) {
                size_t sx = (size_t(p.x) & aoc::bitmask<size_t>(8 * sizeof(p.y))) << (8 * sizeof(p.y));
                size_t sy = size_t(p.y) & aoc::bitmask<size_t>(8 * sizeof(p.y));
                return sx | sy;
            } else {
                size_t sx = size_t(p.x);
                size_t sy = size_t(p.y);
                return sx ^ sy + 0x9e3779b9 + (sx << 6) + (sx >> 2);
            }
        }
    };
}
namespace fmt {
    template <>
    struct formatter<point> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const point& p, FormatContext &ctx) {
            return format_to(ctx.out(), "{{{}, {}}}", p.x, p.y);
        }
    };
}

using point_map = std::unordered_map<point, aoc::computer::memory_value_t>;

static inline void write_point(point_map& map, point p, aoc::computer::memory_value_t v) {
    map.insert_or_assign(p, v);
}

static inline aoc::computer::memory_value_t read_point(const point_map& map, point p, aoc::computer::memory_value_t def = 0) {
    auto it = map.find(p);
    return it == map.end() ? def : it->second;
}

enum orientation {
    up,
    down,
    left,
    right,
};

static inline void advance(point& current_position, orientation& current_orientation, aoc::computer::memory_value_t turn_direction) {
    switch (turn_direction) {
        case 0: {
            switch (current_orientation) {
                case up: {
                    current_orientation = left;
                    current_position.x -= 1;
                    break;
                }
                case down: {
                    current_orientation = right;
                    current_position.x += 1;
                    break;
                }
                case left: {
                    current_orientation = down;
                    current_position.y -= 1;
                    break;
                }
                case right: {
                    current_orientation = up;
                    current_position.y += 1;
                    break;
                }
            }
            break;
        }
        case 1: {
            switch (current_orientation) {
                case up: {
                    current_orientation = right;
                    current_position.x += 1;
                    break;
                }
                case down: {
                    current_orientation = left;
                    current_position.x -= 1;
                    break;
                }
                case left: {
                    current_orientation = up;
                    current_position.y += 1;
                    break;
                }
                case right: {
                    current_orientation = down;
                    current_position.y -= 1;
                    break;
                }
            }
            break;
        }
        default: std::abort();
    }
}

static inline void run(const aoc::computer& computer, point_map& path,
                       aoc::computer::memory_value_t initial_color = 0,
                       orientation initial_orientation = up) {
    aoc::computer c(computer);
    point current_position{0, 0};
    orientation current_orientation{initial_orientation};

    c.expand_memory(1024 * 1024);

    static constexpr const auto bp = [](const aoc::computer& c) -> bool {
        return c.outputs().size() == 2;
    };

    write_point(path, current_position, initial_color);

    while (true) {
        c.add_input(read_point(path, current_position));
        c.execute_with_conditional_breakpoints({bp});
        if (c.is_halted())
            break;

        auto color = c.get_output().value();
        auto direction = c.get_output().value();

        write_point(path, current_position, color);
        advance(current_position, current_orientation, direction);
    }
}

static inline void part1(const aoc::computer& computer) {
    point_map path{};

    run(computer, path);

    fmt::print("{}\n", path.size());
}

static inline void part2(const aoc::computer& computer) {
    point_map path{};
    int minx{std::numeric_limits<int>::max()}, miny{std::numeric_limits<int>::max()};
    int maxx{std::numeric_limits<int>::min()}, maxy{std::numeric_limits<int>::min()};

    run(computer, path, 1);

    fmt::print("DONE!\n");

    for (const auto& [k, v] : path) {
        minx = std::min(minx, k.x);
        miny = std::min(miny, k.y);

        maxx = std::max(maxx, k.x);
        maxy = std::max(maxy, k.y);
    }

    int dx = -minx;
    int dy = -maxy;

    fmt::print("({} {}) - ({} {}) -> ({} {})\n", minx, miny, maxx, maxy, dx, dy);

    std::vector<std::string> message(size_t(maxy - miny), std::string(size_t(maxx - minx), ' '));
    fmt::print("{} {}\n", message.size(), message[0].size());
    for (const auto& [k, v] : path)
        if (v) message[size_t(k.y - dy)][size_t(k.x - dx)] = '#';

    for (const auto& v : message)
        fmt::print("{}\n", v);
}

int main() {
    if constexpr (DEBUG) {
        aoc::computer c{};
        c.add_memory_values("104,0,104,0, 104,0,104,0, 104,1,104,0, 104,1,104,0, 104,0,104,1, 104,1,104,0, 104,1,104,0, 99");
        part1(c);
    }

    auto computer = aoc::computer::read_initial_state();

    part1(computer);
    part2(computer);

    return 0;
}
