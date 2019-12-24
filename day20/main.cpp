#include <grid.h>
#include <pathfind.h>

class Grid : public aoc::grid::Grid {
public:
    virtual inline std::optional<std::string> parse_special_tiles(std::vector<std::string_view>& input_lines) override final {
        static char sz_label[3] = {' ', ' ', '\0'};
        std::unordered_map<std::string, std::vector<aoc::grid::point>> labels{};

        for (int line = 0; line < int(input_lines.size() - 1); line++) {
            auto& l1 = input_lines.at(line);
            auto& l2 = input_lines.at(line + 1);

            for (int col = 0; col < int(l1.size()); col++) {
                auto& c1 = *((char*)l1.data() + col);
                auto& c2 = *((char*)l2.data() + col);

                if (std::clamp(c1, 'A', 'Z') != c1)
                    continue;
                if (std::clamp(c2, 'A', 'Z') != c2)
                    continue;

                sz_label[0] = c1;
                sz_label[1] = c2;

                c1 = ' ';
                c2 = ' ';

                auto& points = labels[sz_label];
                if (line < (int(input_lines.size()) - 2) && input_lines.at(line + 2).at(col) == '.')
                    points.push_back({col, line + 2});
                if (line > 0 && input_lines.at(line - 1).at(col) == '.')
                    points.push_back({col, line - 1});
            }
        }

        for (int line = 0; line < int(input_lines.size() - 1); line++) {
            auto& l1 = input_lines.at(line);

            for (int col = 0; col < int(l1.size()) - 1; col++) {
                auto& c1 = *((char*)l1.data() + col);
                auto& c2 = *((char*)l1.data() + col + 1);

                if (std::clamp(c1, 'A', 'Z') != c1)
                    continue;
                if (std::clamp(c2, 'A', 'Z') != c2)
                    continue;

                sz_label[0] = c1;
                sz_label[1] = c2;

                c1 = ' ';
                c2 = ' ';

                auto& points = labels[sz_label];
                if (col > 0 && l1.at(col - 1) == '.')
                    points.push_back({col - 1, line});
                if (col < (int(l1.size()) - 2) && l1.at(col + 2) == '.')
                    points.push_back({col + 2, line});
            }
        }

        labels_ = std::move(labels);
        return {};
    }

    virtual inline std::vector<aoc::grid::point> special_neighours(aoc::grid::point p) const override final {
        std::vector<aoc::grid::point> points{};

        auto labels = labels_for_point(p);
        for (auto& ll : labels) {
            auto pfl = points_for_label(ll);
            for (auto& pf : pfl) {
                if (pf == p) continue;
                points.push_back(pf);
            }
        }

        return points;
    }

    inline const auto& labels() const { return labels_; }

    inline std::vector<aoc::grid::point> points_for_label(std::string_view label) const {
        std::vector<aoc::grid::point> ret{};
        for (const auto& [ll, lp] : labels_) {
            if (ll != label) continue;
            ret.insert(std::end(ret), std::begin(lp), std::end(lp));
        }
        return ret;
    }

    inline std::vector<std::string_view> labels_for_point(aoc::grid::point p) const {
        std::vector<std::string_view> ret{};
        for (const auto& [ll, lp] : labels_) {
            for (const auto& pp : lp)
                if (pp == p) ret.push_back(ll);
        }
        return ret;
   }

private:
    std::unordered_map<std::string, std::vector<aoc::grid::point>> labels_{};
};

namespace fmt {
    template <>
    struct formatter<Grid> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const Grid& grid, FormatContext &ctx) {
            const auto& labels = grid.labels();
            if (!labels.empty())
                format_to(ctx.out(), "{}\n", labels);
            format_to(ctx.out(), "{}", *((const aoc::grid::Grid*)&grid));
            return ctx.out();
        }
    };
}

int main() {
    Grid grid{};

    if (auto ms = grid.load_from_file(std::cin); ms) {
        fmt::print("Failed to load grid: {}\n", ms.value());
        return 1;
    }

    auto start = grid.points_for_label("AA").front();
    auto end = grid.points_for_label("ZZ").front();

    aoc::pathfinding::AStar astar{};
    astar.init(grid);

    const auto& path = astar.find_path(start, end);
    fmt::print("{}\n", path.size());

    return 0;
}
