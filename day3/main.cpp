#include <aoc.h>

struct point {
    int x;
    int y;
};
using maybe_point = std::optional<point>;

static bool operator==(const point& A, const point& B) {
    return A.x == B.x && A.y == B.y;
}

static std::ostream& operator<<(std::ostream& out, const point& p) {
    return out << "{" << p.x << ", " << p.y << "}";
}

int manhattan_distance(const point& A, const point& B) {
    return std::abs(A.x - B.x) + std::abs(A.y - B.y);
}

enum class aa_orientation {
    vertical,
    horizontal,
};

struct aa_segment {
    point A;
    point B;
};

[[maybe_unused]] static std::ostream& operator<<(std::ostream& out, const aa_segment& seg) {
    return out << "{" << seg.A << ", " << seg.B << "}";
}

static aa_orientation segment_orientation(const aa_segment& seg) {
    return seg.A.x == seg.B.x ? aa_orientation::vertical : aa_orientation::horizontal;
}

static maybe_point segment_intersection(const aa_segment& s1, const aa_segment& s2) {
    auto o1 = segment_orientation(s1);
    if (o1 == segment_orientation(s2)) return {};

    const auto in_interval = [](int x, int a, int b) -> bool {
        return x >= std::min(a, b) && x <= std::max(a, b);
    };

    if (o1 == aa_orientation::vertical && in_interval(s1.A.x, s2.A.x, s2.B.x) && in_interval(s2.A.y, s1.A.y, s1.B.y)) {
        return point{s1.A.x, s2.A.y};
    }
    if (in_interval(s2.A.x, s1.A.x, s1.B.x) && in_interval(s1.A.y, s2.A.y, s2.B.y)) {
        return point{s2.A.x, s1.A.y};
    }
    return {};
}

static int segment_length(const aa_segment& seg) {
    return manhattan_distance(seg.A, seg.B);
}

std::vector<aa_segment> parse_path(std::string_view data) {
    point A{0, 0}, B;
    std::vector<aa_segment> ret;
    for (auto s : aoc::str_split(data, ',')) {
        s = aoc::trim(s);

        char dir = s[0];
        int delta;

        s.remove_prefix(1);
        auto r = std::from_chars(s.cbegin(), s.cend(), delta);
        if (r.ec != std::errc()) break;

        switch (dir) {
            case 'U': {
                B = point{A.x, A.y + delta};
                break;
            }
            case 'D': {
                B = point{A.x, A.y - delta};
                break;
            }
            case 'R': {
                B = point{A.x + delta, A.y};
                break;
            }
            case 'L': {
                B = point{A.x - delta, A.y};
                break;
            }
        }

        ret.push_back({A, B});
        A = B;
    }
    return ret;
}

static std::pair<int, int> find_intersections(const std::vector<aa_segment>& p1, const std::vector<aa_segment>& p2) {
    int old_dist = std::numeric_limits<int>::max();
    int min_steps = std::numeric_limits<int>::max();
    point origin{0, 0};
    point ret{0, 0};

    const auto add_segment = [](int steps, const aa_segment& seg) -> int {
        return steps + segment_length(seg);
    };

    for (const auto& p2_seg: p2) {
        for (const auto& p1_seg : p1) {
            auto maybe_intersection = segment_intersection(p1_seg, p2_seg);
            if (!maybe_intersection)
                continue;

            auto intersection = maybe_intersection.value();
            if (intersection == origin)
                continue;

            int new_dist = manhattan_distance(origin, intersection);
            if (new_dist < old_dist) {
                ret = intersection;
                old_dist = new_dist;
            }

            int steps_p1 = std::accumulate(p1.data(), &p1_seg, manhattan_distance(intersection, p1_seg.A), add_segment);
            int steps_p2 = std::accumulate(p2.data(), &p2_seg, manhattan_distance(intersection, p2_seg.A), add_segment);
            min_steps = std::min(min_steps, steps_p1 + steps_p2);
        }
    };

    return {manhattan_distance(origin, ret), min_steps};
}

static auto find_intersections(std::string_view p1, std::string_view p2) {
    return find_intersections(parse_path(p1), parse_path(p2));
}

int main() {
    if constexpr (DEBUG) {
        fmt::print("{}\n", find_intersections("R8,U5,L5,D3",
                                              "U7,R6,D4,L4"));
        fmt::print("{}\n", find_intersections("R75,D30,R83,U83,L12,D49,R71,U7,L72",
                                              "U62,R66,U55,R34,D71,R55,D58,R83"));
        fmt::print("{}\n", find_intersections("R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51",
                                              "U98,R91,D20,R16,D67,R40,U7,R15,U6,R7"));
    }

    std::string sp1;
    std::string sp2;
    std::getline(std::cin, sp1);
    std::getline(std::cin, sp2);
    auto [min_distance, min_steps] = find_intersections(sp1, sp2);
    fmt::print("{}\n{}\n", min_distance, min_steps);
    return 0;
}
