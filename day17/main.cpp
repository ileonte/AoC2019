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
        if (is_vertical()) return p2_.y - p1_.y + 1;
        return p2_.x - p1_.x + 1;
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
                        buff = fmt::format("R,{}", length() - 1);
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("L,{}", length() - 1);
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("R,R,{}", length() - 1);
                        break;
                    }
                    case RIGHT:
                        buff = fmt::format("{}", length() - 1);
                        break;
                }
            } else {
                assert(current_position == p2_);
                new_dir = LEFT;
                new_pos = p1_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("L,{}", length() - 1);
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("R,{}", length() - 1);
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("{}", length() - 1);
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("L,L,{}", length() - 1);
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
                        buff = fmt::format("L,L,{}", length() - 1);
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("{}", length() - 1);
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("L,{}", length() - 1);
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("R,{}", length() - 1);
                        break;
                    }
                }
            } else {
                assert(current_position == p2_);
                new_dir = UP;
                new_pos = p1_;
                switch (current_direction) {
                    case UP: {
                        buff = fmt::format("{}", length() - 1);
                        break;
                    }
                    case DOWN: {
                        buff = fmt::format("L,L,{}", length() - 1);
                        break;
                    }
                    case LEFT: {
                        buff = fmt::format("R,{}", length() - 1);
                        break;
                    }
                    case RIGHT: {
                        buff = fmt::format("L,{}", length() - 1);
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
    static constexpr const size_t program_memory_size = 128 * 1024;

    std::vector<std::vector<char>> data;
    std::string original_program;
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
                if (in_segment && p1 != p2)
                    ret.emplace_back(p1, p2);
                in_segment = false;
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

    inline auto run_cleaning_program(const std::array<std::string, 5>& program) {
        comp.reset(comp.memory().size() - 100, original_program.size());
        comp.clear();
        comp.add_memory_values(original_program);
        comp.expand_memory(scaffolding::program_memory_size);
        comp.mem_ref(0, aoc::computer::AM_IMMEDIATE) = 2;
        for (const auto& line : program) {
            auto sv_line = std::string_view(line);
            while (sv_line.back() == ',') sv_line.remove_suffix(1);
            for (auto ch : sv_line)
                comp.add_input(ch);
            comp.add_input('\n');
        }
        if constexpr (DEBUG) {
            fmt::print("INPUTS: {}\n", comp.inputs());
            for (char ch : comp.inputs())
                fmt::print("{}", ch);
            fmt::print("END INPUTS\n");
        }
        comp.execute();
        comp.mem_ref(0, aoc::computer::AM_IMMEDIATE) = 1;
        return comp.outputs().back();
    }

    static inline scaffolding read(std::istream& in = std::cin) {
        scaffolding ret{};
        std::getline(in, ret.original_program);
        ret.comp = aoc::computer();
        ret.comp.add_memory_values(ret.original_program);
        ret.comp.expand_memory(scaffolding::program_memory_size);
        ret.comp.execute();

        std::vector<char> line{};
        line.reserve(512);

        if constexpr (DEBUG) {
            for (char ch : ret.comp.outputs())
                fmt::print("{}", ch);
        }

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
                    if (!aoc::trim(std::string_view(line.data(), line.size())).empty())
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

    static constexpr const auto replace_function = [](const std::string& s, std::string_view f, std::string_view fn) -> std::string {
        std::string ret(s);
        size_t idx = ret.find(f.data(), 0, f.size());
        while (idx != ret.npos) {
            ret.replace(idx, f.size(), fn);
            idx = ret.find(f.data(), idx + fn.size(), f.size());
        }
        return ret;
    };

    static constexpr const auto is_final_program = [](std::string_view p) -> bool {
        if (p.size() > 20) return false;
        for (auto ch : p) {
            switch (ch) {
                case 'A': [[fallthrough]];
                case 'B': [[fallthrough]];
                case 'C': [[fallthrough]];
                case ',': continue;
                default: return false;
            }
        }
        return true;
    };

    std::array<std::string, 5> program_input{
        "", /* main movement routine */
        "", /* A */
        "", /* B */
        "", /* C */
        "n" /* display control */
    };

    auto prog_full = s.walk_instructions();
    auto functions_A = possible_functions(prog_full);
    for (auto f_A : functions_A) {
        auto prog_A = replace_function(prog_full, f_A, "A,");
        if (is_final_program(prog_A)) {
            program_input[0] = std::move(prog_A);
            program_input[1] = f_A;
            goto done;
        }

        auto functions_B = possible_functions(prog_A);
        for (auto f_B : functions_B) {
            auto prog_B = replace_function(prog_A, f_B, "B,");
            if (is_final_program(prog_B)) {
                program_input[0] = std::move(prog_B);
                program_input[1] = f_A;
                program_input[2] = f_B;
                goto done;
            }

            auto functions_C = possible_functions(prog_B);
            for (auto f_C : functions_C) {
                auto prog_C = replace_function(prog_B, f_C, "C,");

                if (is_final_program(prog_C)) {
                    program_input[0] = std::move(prog_C);
                    program_input[1] = f_A;
                    program_input[2] = f_B;
                    program_input[3] = f_C;
                    goto done;
                }
            }
        }
    }

done:
    auto ret = s.run_cleaning_program(program_input);
    if constexpr (DEBUG) {
        for (auto ch : s.comp.outputs())
            fmt::print("{}", char(ch));

        std::string prog_rebuilt{};
        for (auto ch : program_input[0]) {
            switch (ch) {
                case 'A': [[fallthrough]];
                case 'B': [[fallthrough]];
                case 'C': {
                    prog_rebuilt.append(program_input[ch - 'A' + 1]);
                    break;
                }
                default: continue;
            }
        }
        fmt::print("ORIGINAL: {}\n", prog_full);
        fmt::print("REBUILT : {}\n", prog_rebuilt);
        fmt::print("SAME    : {}\n", prog_full == prog_rebuilt);
    }
    fmt::print("{}\n", ret);
}

int main() {
    auto s = scaffolding::read();

    part1(s);
    part2(s);

    return 0;
}
