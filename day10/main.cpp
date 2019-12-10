#include <aoc.h>

using fraction = std::pair<int, int>;
using point = std::pair<int, int>;

static inline bool operator<(const fraction& f1, const fraction& f2) {
    auto lcm = aoc::lcm(f1.second, f2.second);
    return (f1.first * (lcm / f2.second)) < (f2.first * (lcm / f1.second));
}

static inline fraction make_fraction(int d, int n) {
    int g = aoc::gcd(d, n);
    return {d / g, n / g};
}

static inline void add_points(std::vector<point>& points, int row, std::string_view data) {
    for (int i = 0; i < int(data.size()); i++) {
        if (data.at(size_t(i)) == '#') points.push_back({i, row});
    }
}

static inline fraction get_slope(point p1, point p2) {
    static constexpr const int maxint = std::numeric_limits<int>::max();
    static constexpr const int minint = std::numeric_limits<int>::min();

    int dx = p1.first - p2.first;
    int dy = p1.second - p2.second;
    if (!dx) return {1, p2.second < p1.second ? minint : maxint};
    else if (!dy) return {p2.first < p1.first ? minint : maxint, 1};
    return make_fraction(dy, dx);
}

static inline auto get_distance(point p1, point p2) {
    auto dx = p1.first - p2.first;
    auto dy = p1.second - p2.second;
    return dx * dx + dy * dy;
}

static inline auto get_quadrant(point center, point p) {
    if (center == p)
        return 5;
    if (p.first == center.first)
        return p.second < center.second ? 1 : 3;
    if (p.second == center.second)
        return p.first < center.first ? 4 : 2;
    if (p.first > center.first && p.second < center.second) return 1;
    if (p.first > center.first && p.second > center.second) return 2;
    if (p.first < center.first && p.second > center.second) return 3;
    assert(p.first < center.first && p.second < center.second);
    return 4;
}

static inline void topo_sort(point center, std::vector<point>& points) {
    static constexpr const auto is_infinite = [](fraction f) -> bool {
        static constexpr const int maxint = std::numeric_limits<int>::max();
        static constexpr const int minint = std::numeric_limits<int>::min();
        return ((f.first == maxint || f.first == minint) || (f.second == maxint || f.second == minint));
    };

    std::sort(std::begin(points), std::end(points), [&](point p1, point p2) -> bool {
        auto q1 = get_quadrant(center, p1);
        auto q2 = get_quadrant(center, p2);
        if (q1 != q2) return q1 < q2;

        auto s1 = get_slope(center, p1);
        auto s2 = get_slope(center, p2);
        auto d1 = get_distance(center, p1);
        auto d2 = get_distance(center, p2);
        if (s1 == s2) return d1 < d2;

        if (is_infinite(s1)) return true;
        if (is_infinite(s2)) return false;

        switch (q1) {
            case 1: {
                if (p1.first == p2.first) return p1.second < p2.second;
                return p1.first < p2.first;
            }
            case 2: {
                if (p1.first == p2.first) return p1.second < p2.second;
                return p1.first > p2.first;
            }
            case 3: {
                if (p1.first == p2.first) return p1.second > p2.second;
                return p1.first > p2.second;
            }
            default: {
                if (p1.first == p2.first) return p1.second < p2.second;
                return p1.second < p2.second;
            }
        }
    });
}

static inline size_t part1(const std::vector<point>& points) {
    size_t ret_count{0};
    point ret_point{-1, -1};
    size_t ret{0};

    for (size_t i = 0; i < points.size(); i++) {
        std::set<fraction> slopes{};
        auto& p1 = points.at(i);

        for (size_t j = 0; j < points.size(); j++) {
            if (i == j) continue;
            auto& p2 = points.at(j);
            slopes.insert(get_slope(p1, p2));
        }

        if (std::max(ret_count, slopes.size()) == slopes.size()) {
            ret_count = slopes.size();
            ret_point = points.at(i);
            ret = i;
        }
    }

    fmt::print("{}\n", ret_count);
    return ret;
}

static inline void part2(const std::vector<point>& points, size_t center_idx) {
    using moop = std::pair<fraction, std::deque<point>>;
    auto center = points.at(center_idx);
    std::vector<moop> sorted_points;

    static const auto add_point = [&](point p) {
        auto slope = get_slope(center, p);

        if (p.first == center.first)
            fmt::print("{} ", p);

        for (auto& it : sorted_points) {
            if (it.first == slope) {
                it.second.push_back(p);
                return;
            }
        }
        sorted_points.push_back({slope, {p}});
    };

    for (auto p : points) {
        if (p == center) continue;
        add_point(p);
    }
    fmt::print("\n");

    for (auto& it : sorted_points) {
        std::sort(std::begin(it.second), std::end(it.second), [&](point p1, point p2) {
            return get_distance(center, p1) < get_distance(center, p2);
        });
    }
    std::sort(std::begin(sorted_points), std::end(sorted_points), [&](const moop& v1, const moop& v2) -> bool {
        assert(v1.second.size() > 0 && v2.second.size() > 0);

        const auto& p1 = v1.second.front();
        const auto& p2 = v2.second.front();
        auto q1 = get_quadrant(center, p1);
        auto q2 = get_quadrant(center, p2);

        if (q1 == 1 && (p1.first == center.first || p2.first == center.first)) {
            fmt::print("{} {}\n", p1, p2);
        }

        if (q1 != q2) return q1 < q2;

        switch (q1) {
            case 1: {
                if (p1.first == p2.first) return p1.second < p2.second;
                return p1.first < p2.first;
            }
            case 2: {
                if (p1.first == p2.first) return p1.second < p2.second;
                return p1.first > p2.first;
            }
            case 3: {
                if (p1.first == p2.first) return p1.second > p2.second;
                return p1.first > p2.first;
            }
            default: {
                assert(q1 == 4);
                if (p1.first == p2.first) return p1.second > p2.second;
                return p1.first < p2.first;
            }
        }
    });

    size_t destroyed{0};
    point ret{-1, -1};
    static const auto do_round = [&]() -> std::pair<bool, bool> {
        size_t round_destroyed{0};
        for (auto& v : sorted_points) {
            if (!v.second.size()) continue;

            round_destroyed++;
            destroyed++;
            if (destroyed == 200) {
                ret = v.second.front();
                return {true, false};
            }

            v.second.pop_front();
        }
        return {false, round_destroyed > 0};
    };

    auto r = do_round();
    while (!r.first && r.second)
        r = do_round();

    fmt::print("{}\n", ret);
}

int main() {
    if constexpr (DEBUG) {
        std::vector<point> points{{11, 5}, {11, 10}, {11, 2}, {11, 12}, {11, 1}, {11, 9}, {11, 11}, {11, 3}, {11, 4}, {11, 8}, {11, 7}, {11, 6}};
        topo_sort({11, 13}, points);
        fmt::print("{}\n", points);
        fmt::print("{}\n", get_quadrant({11, 13}, points[0]));
    }
    std::vector<point> points{};
    std::string in{};
    int row{0};
    while (std::getline(std::cin, in)) add_points(points, row++, in);
    auto idx = part1(points);
    part2(points, idx);
    return 0;
}
