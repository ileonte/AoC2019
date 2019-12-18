#include <aoc.h>

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

class maze {
public:
    struct PoI {
        char what;
        point pos;

        friend inline std::ostream& operator<<(std::ostream& out, PoI p) {
            return out << "POI('" << p.what << "' at " << p.pos << ")";
        }
    };

    maze() = default;
    maze(const maze&) = default;
    maze(maze&&) = default;

    maze& operator=(const maze&) = default;
    maze& operator=(maze&&) = default;

    static inline std::optional<maze> read(std::istream& in = std::cin) {
        std::string s_in{};
        size_t height{0};
        size_t width{0};
        std::string data{};

        while (std::getline(in, s_in)) {
            auto sv = aoc::trim(s_in);
            if (!width)
                width = sv.size();
            if (!width)
                continue;
            if (width != sv.size()) {
                fmt::print(::stderr, "INVALID LINE: '{}'\nSIZE: {}, EXPECTED SIZE: {}\n", sv, sv.size(), width);
                return {};
            }
            height += 1;
            data.append(sv.data(), sv.size());
        }

        maze ret{};
        ret.data_ = std::move(data);
        ret.width_ = width;
        ret.height_ = height;
        return ret;
    }

    static inline auto read(const std::string& s) {
        std::stringstream ss(s);
        return maze::read(ss);
    }

    void print() const {
        for (size_t i = 0; i < height_; i++)
            fmt::print("{}\n", aoc::substr(data_, i * width_, width_));
    }

    inline auto width() const { return width_; }
    inline auto height() const { return height_; }
    inline const auto& data() const { return data_; }

    auto* p_at(size_t x, size_t y) { return data_.data() + y * width_ + x; }
    const auto* p_at(size_t x, size_t y) const { return data_.data() + y * width_ + x; }
    auto* p_at(point p) { return p_at(p.x, p.y); }
    auto& r_at(size_t x, size_t y) { return *p_at(x, y); };
    const auto& r_at(size_t x, size_t y) const { return *p_at(x, y); };


    static inline char next_fill_char() {
        static char chars[] = {'*', '+'};
        static size_t counter = 0;
        return chars[counter++ % 2];
    }

    inline std::vector<PoI> get_pois(std::optional<point> maybe_start = {}) {
        point start{};
        if (!maybe_start) {
            auto idx = data_.find('@');
            if (idx == data_.npos) {
                fmt::print("NEED START POINT!\n");
                std::abort();
            }
            start = point{idx % width_, idx / width_};
        } else
            start = maybe_start.value();

        std::vector<PoI> ret{};
        char fill_ch{maze::next_fill_char()};

        static const auto recurse = [this](const auto& self, std::vector<PoI>& pois, char fill_ch, point pos) {
        };

        fmt::print("{}\n", start);

        return ret;
    }

private:
    std::string data_{};
    size_t width_{0};
    size_t height_{0};
};

int main() {
    if constexpr (DEBUG) {
        static constexpr char const* test_cases[] = {
            "#########\n"
            "#b.A.@.a#\n"
            "#########",

            "########################\n"
            "#f.D.E.e.C.b.A.@.a.B.c.#\n"
            "######################.#\n"
            "#d.....................#\n"
            "########################",

            "########################\n"
            "#...............b.C.D.f#\n"
            "#.######################\n"
            "#.....@.a.B.c.d.A.e.F.g#\n"
            "########################",

            "#################\n"
            "#i.G..c...e..H.p#\n"
            "########.########\n"
            "#j.A..b...f..D.o#\n"
            "########@########\n"
            "#k.E..a...g..B.n#\n"
            "########.########\n"
            "#l.F..d...h..C.m#\n"
            "#################",

            "########################\n"
            "#@..............ac.GI.b#\n"
            "###d#e#f################\n"
            "###A#B#C################\n"
            "###g#h#i################\n"
            "########################"
        };

        for (auto p : test_cases) {
            if (auto mm = maze::read(p); mm) {
                mm->get_pois();
            }
        }
    }

    /*
     * devices = json_parse(devices_str);
     * for (int i = 0; i < devices.size(); i++) {
     *    device = devices[i];
     *    macs = device.macs;
     *    printf("This device has %d macs\n", macs.size());
     * }
     *
     */

    return 0;
}
