#include <aoc.h>

using fraction = std::pair<int, int>;
using point = std::pair<int, int>;

static inline fraction make_fraction(int d, int n) {
    int g = aoc::gcd(d, n);
    return {d / g, n / g};
}

static inline void add_points(std::vector<point>& points, int row, std::string_view data) {
    for (int i = 0; i < int(data.size()); i++) {
        if (data.at(i) == '#') points.push_back({i, row});
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

static inline size_t part1(const std::vector<point>& points) {
    size_t ret_count{0};
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
            ret = i;
        }
    }

    fmt::print("{}\n", ret_count);
    return ret;
}

static inline void part2(const std::vector<point>& in_points, size_t center_idx) {
    std::vector<point> points(in_points);
    point ret{-1, -1};
    size_t destroyed_counter{0};

    const auto remove_point = [&](size_t idx) {
        auto it = std::begin(points);
        std::advance(it, idx);
        points.erase(it);
    };

    point center = points.at(center_idx);
    remove_point(center_idx);

    const auto do_rotation = [&]() -> std::pair<bool, bool> {
        std::set<fraction> slopes{};
        size_t destroyed_now{0};

        for (auto it = std::begin(points); it != std::end(points);) {
            auto slope = get_slope(center, *it);
            auto [sit, inserted] = slopes.insert(slope);
            if (inserted) {
                destroyed_counter++;
                destroyed_now++;
                if (destroyed_counter == 200) {
                    ret = *it;
                    return {true, false};
                }
                points.erase(it);
            } else {
                std::advance(it, 1);
            }
        }
        return {false, destroyed_now > 0};
    };

    auto r = do_rotation();
    while (!r.first && r.second)
        r = do_rotation();

    fmt::print("{}\n", ret.first * 100 + ret.second);
}

int main() {
    std::vector<point> points{};
    std::string in{};
    int row{0};
    while (std::getline(std::cin, in)) add_points(points, row++, in);
    auto idx = part1(points);
    part2(points, idx);
    return 0;
}
