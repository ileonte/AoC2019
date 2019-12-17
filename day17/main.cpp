#include <computer.h>

enum direction : char {
    UP = '^',
    DOWN = 'v',
    LEFT = '<',
    RIGHT = '>',
};

struct point {
    size_t x;
    size_t y;

    friend inline bool operator==(const point& p1, const point& p2) {
        return p1.x == p2.x && p1.y == p2.y;
    }
    friend inline bool operator!=(const point& p1, const point& p2) {
        return !operator==(p1, p2);
    }

    friend inline std::ostream& operator<<(std::ostream& out, const point& p) {
        return out << "{" << p.x << ", " << p.y << "}";
    }
};

class aab_segment {
private:
    point p1_;
    point p2_;

public:
    aab_segment(point p1, point p2) {
        if (p1.x == p2.x) {
            if (p1.y < p2.y) {
                p1_ = p1;
                p2_ = p2;
            } else {
                p1_ = p2;
                p2_ = p1;
            }
        } else if (p1.y == p2.y) {
            if (p1.x < p2.x) {
                p1_ = p1;
                p2_ = p2;
            } else {
                p1_ = p2;
                p2_ = p1;
            }
        } else {
            fmt::print(::stderr, "BAD SEGMENT: {}<->{}\n", p1, p2);
            std::abort();
        }
    }
    aab_segment(const aab_segment&) = default;
    aab_segment(aab_segment&&) = default;

    aab_segment& operator=(const aab_segment&) = default;
    aab_segment& operator=(aab_segment&&) = default;

    inline bool ends_with(point p) const {
        return p1_ == p || p2_ == p;
    }

    inline bool is_vertical() const {
        return p1_.x == p2_.x;
    }
    inline bool is_horizontal() const {
        return p1_.y == p2_.y;
    }

    size_t length() const {
        if (is_vertical()) return p2_.y - p1_.y;
        return p2_.x - p1_.x;
    }

    std::string_view walk_instructions(direction& current_direction, point& current_position) {
        static std::string buff{};
        direction new_dir{};
        point new_pos{};
        if (is_horizontal()) {
            if (current_position == p1_) {
                new_dir = RIGHT;
                new_pos = p2_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("R,{}", length());
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("L,{}", length());
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("R,R,{}", length());
                        break;
                    }
                    case RIGHT:
                        buff = fmt::format("{}", length());
                        break;
                }
            } else {
                assert(current_position == p2_);
                new_dir = LEFT;
                new_pos = p1_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("L,{}", length());
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("R,{}", length());
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("{}", length());
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("L,L,{}", length());
                        break;
                    }
                }
            }
        } else {
            assert(is_vertical());
            if (current_position == p1_) {
                new_dir = DOWN;
                new_pos = p2_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("L,L,{}", length());
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("{}", length());
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("L,{}", length());
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("R,{}", length());
                        break;
                    }
                }
            } else {
                assert(current_position == p2_);
                new_dir = UP;
                new_pos = p1_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("{}", length());
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("L,L,{}", length());
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("R,{}", length());
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("L,{}", length());
                        break;
                    }
                }
            }
        }

        current_direction = new_dir;
        current_position = new_pos;
        return buff;
    }

    friend inline bool operator==(const aab_segment& s1, const aab_segment& s2) {
        return (s1.p1_ == s2.p1_ && s1.p2_ == s2.p2_) || (s1.p1_ == s2.p2_ && s1.p2_ == s2.p1_);
    }

    friend inline std::ostream& operator<<(std::ostream& out, const aab_segment& s) {
        return out << "{" << s.p1_ << "<->" << s.p2_ << "}";
    }
};

struct scaffolding {
    std::vector<std::vector<char>> data;
    aoc::computer comp;
    point robot_position;
    direction robot_direction;

    inline void print() const {
        for (size_t y = 0; y < data.size(); y++) {
            const auto& line = data.at(y);
            for (size_t x = 0; x < line.size(); x++) {
                if (x == robot_position.x && y == robot_position.y)
                    fmt::print("{}", char(robot_direction));
                else
                    fmt::print("{}", line.at(x));
            }
            fmt::print("\n");
        }
    }

    inline std::vector<aab_segment> segments() const {
        std::vector<aab_segment> ret{};

        constexpr static const auto do_segment_detect = [](const std::vector<std::vector<char>>& data, size_t line, size_t col, std::vector<aab_segment>& ret) {
            static bool in_segment{false};
            static point p1{};
            static point p2{};

            if (line >= data.size()) {
                if (in_segment && p1 != p2) {
                    in_segment = false;
                    ret.emplace_back(p1, p2);
                }
                return;
            }

            if (data[line][col] != '#') {
                if (in_segment) {
                    if (p1 != p2) ret.emplace_back(p1, p2);
                    in_segment = false;
                }
            } else {
                if (!in_segment) {
                    in_segment = true;
                    p1 = point{col, line};
                }
                p2 = point{col, line};
            }
        };

        /* horizontal segments */
        for (size_t line = 0; line < data.size(); line++) {
            for (size_t col = 0; col < data.at(line).size(); col++)
                do_segment_detect(data, line, col, ret);
            do_segment_detect(data, data.size(), 0, ret);
        }

        /* vertical segments */
        for (size_t col = 0; col < data.at(0).size(); col++) {
            for (size_t line = 0; line < data.size(); line++)
                do_segment_detect(data, line, col, ret);
            do_segment_detect(data, data.size(), 0, ret);
        }

        return ret;
    }

    inline std::string walk_instructions() const {
        auto segments = this->segments();

        static constexpr const auto get_segment = [](std::vector<aab_segment>& segments, point current_position) -> std::optional<aab_segment> {
            for (auto it = segments.begin(); it != segments.end(); it++) {
                if (it->ends_with(current_position)) {
                    auto ret = *it;
                    segments.erase(it);
                    return ret;
                }
            }
            return {};
        };

        auto current_position = this->robot_position;
        auto current_direction = this->robot_direction;
        std::string program{};
        while (!segments.empty()) {
            auto ms = get_segment(segments, current_position);
            if (!ms) {
                fmt::print(::stderr, "NO KNOWN SEGMENT FOR {}\n", current_position);
                std::abort();
            }
            program.append(ms->walk_instructions(current_direction, current_position));
            program += ',';
        }

        return program;
    }

    static inline scaffolding read() {
        scaffolding ret{};
        ret.comp = aoc::computer::read_initial_state();
        ret.comp.expand_memory(128 * 1024);
        ret.comp.execute();

        std::vector<char> line{};
        line.reserve(512);

        for (char ch : ret.comp.outputs()) {
            switch (ch) {
                case '#': [[fallthrough]];
                case '.': {
                    line.push_back(ch);
                    break;
                }
                case '^': [[fallthrough]];
                case '<': [[fallthrough]];
                case '>': [[fallthrough]];
                case 'v': {
                    ret.robot_position.x = line.size();
                    ret.robot_position.y = ret.data.size();
                    ret.robot_direction = direction(ch);
                    line.push_back('#');
                    break;
                }
                case '\n': [[fallthrough]];
                case '\r': {
                    ret.data.emplace_back(std::move(line));
                    line = std::vector<char>();
                    line.reserve(512);
                    break;
                }
                default: {
                    fmt::print("BAD INPUT: '{}' ({})\n", ch, int(ch));
                    std::abort();
                }
            }
        }

        return ret;
    }
};

static inline void part1(const scaffolding& s) {
    const auto v_at = [&s](size_t x, size_t y) -> const char& {
        return s.data.at(y).at(x);
    };

    size_t ret{0};

    for (size_t y = 1; y < s.data.size() - 1; y++) {
        for (size_t x = 1; x < s.data[y].size() - 1; x++) {
            if (v_at(x, y) != '#') continue;
            if (v_at(x - 1, y) == '#' && v_at(x + 1, y) == '#' && v_at(x, y - 1) == '#' && v_at(x, y + 1) == '#')
                ret += (x * y);
        }
    }

    fmt::print("{}\n", ret);
}

static inline void part2(scaffolding& s) {
    auto program = s.walk_instructions();

    static constexpr const auto possible_functions = [](const std::string& s) -> std::vector<std::string_view> {
        auto parts = aoc::str_split(s, ',');
        std::vector<std::string_view> ret{};

        size_t start_idx{0};
        for (; start_idx < parts.size(); start_idx++) {
            if (parts[start_idx].at(0) != 'A' && parts[start_idx].at(0) != 'B' && parts[start_idx].at(0) != 'C')
                break;
        }
        if (start_idx >= parts.size())
            return ret;

        size_t size{0};
        for (size_t i = start_idx; i < parts.size() - 1; i += 2) {
            if (parts[i].at(0) == 'A' || parts[i].at(0) == 'B' || parts[i].at(0) == 'C')
                break;
            if (parts[i + 1].at(0) == 'A' || parts[i + 1].at(0) == 'B' || parts[i + 1].at(0) == 'C')
                break;

            size += (parts[i].size() + parts[i + 1].size() + 2);
            if (size <= 20) ret.push_back(aoc::substr(s, 2 * start_idx, size));
            else break;
        }

        return ret;
    };

    fmt::print("{}\n", program);
    auto functions = possible_functions(program);
    for (auto f : functions)
        fmt::print("{}\n", f);
}

int main() {
    auto s = scaffolding::read();
    s.print();

    part1(s);
    part2(s);

    return 0;
}
