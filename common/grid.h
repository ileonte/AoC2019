#ifndef GRID_H
#define GRID_H

#include "aoc.h"

namespace aoc::grid {
    struct point {
        int x{-1};
        int y{-1};

        inline bool operator==(const point& other) const {
            return x == other.x && y == other.y;
        }
    };

    enum class tile_type {
        wall,
        floor,
    };

    class Grid {
    public:
        virtual inline std::optional<std::string> parse_special_tiles(std::vector<std::string_view>&) {
            return {};
        }
        virtual inline std::vector<point> special_neighours(point) const {
            return {};
        }

        inline bool contains(point p) const {
            return !is_empty() && (std::clamp(p.x, 0, width() - 1) == p.x && std::clamp(p.y, 0, height() - 1) == p.y);
        }
        inline bool contains(int x, int y) const {
            return contains({x, y});
        }

        inline bool is_blocked(point p) const {
            if (!contains(p)) return true;
            return grid_[p.y][p.x] != tile_type::floor;
        }
        inline bool is_blocked(int x, int y) const { return is_blocked({x, y}); }

        inline void clear() {
            grid_.clear();
            labels_.clear();
        }

        inline std::optional<std::string> load_from_file(std::istream& in = std::cin) {
            std::string buffer{std::istreambuf_iterator{in}, {}};
            auto input_lines = aoc::str_split(buffer, '\n');
            std::vector<std::vector<tile_type>> lines;
            int line_count{1};

            while (input_lines.size() > 0 && input_lines.back().empty())
                input_lines.pop_back();
            if (!input_lines.size())
                return "Found no non-empty lines in input";

            for (size_t i = 1; i < input_lines.size(); i++) {
                if (input_lines.at(i).size() != input_lines.at(i - 1).size())
                    return fmt::format("Invalid line length at line {}: expecting {}, got {}",
                                       i + 1,
                                       input_lines.at(i - 1).size(),
                                       input_lines.at(i).size());
            }

            parse_special_tiles(input_lines);

            for (auto& line_view : input_lines) {
                if (!lines.empty()) {
                    if (line_view.size() != lines.back().size())
                        return fmt::format("Line {} has a different length to the previous line ({})",
                                           line_view.size(), lines.back().size());

                }

                int col_count{1};
                std::vector<tile_type> line{};
                line.reserve(line_view.size());
                for (char c : line_view) {
                    switch (c) {
                        case ' ': [[fallthrough]];
                        case '#': {
                            line.push_back({});
                            break;
                        }
                        case '.': {
                            line.push_back(tile_type::floor);
                            break;
                        }
                        default: return fmt::format("Invalid char '{}' encountered at line {}, column {}",
                                                    c, line_count, col_count);
                    }
                }
                lines.push_back(std::move(line));

                line_count++;
            }

            grid_ = std::move(lines);

            return {};
        }

        inline bool is_empty() const { return grid_.empty(); }
        inline int width() const { return is_empty() ? 0 : grid_.front().size(); }
        inline int height() const { return is_empty() ? 0 : grid_.size(); }

        inline const auto& raw_grid() const { return grid_; }

        inline const auto& at(int x, int y) const {
            assert(!is_empty());
            assert(std::clamp(x, 0, width() - 1) == x);
            assert(std::clamp(y, 0, height() - 1) == y);
            return grid_[y][x];
        }

    private:
        std::vector<std::vector<tile_type>> grid_{};
        std::unordered_map<std::string, std::vector<point>> labels_{};
    };
}

namespace fmt {
    template <>
    struct formatter<aoc::grid::Grid> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const aoc::grid::Grid& grid, FormatContext &ctx) {
            const auto& lines = grid.raw_grid();
            for (const auto& line : lines) {
                for (auto v : line)
                    format_to(ctx.out(), "{}", v == aoc::grid::tile_type::wall ? '#' : '.');
                format_to(ctx.out(), "\n");
            }
            return ctx.out();
        }
    };

    template <>
    struct formatter<aoc::grid::point> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const aoc::grid::point& p, FormatContext &ctx) {
            return format_to(ctx.out(), "{{{}, {}}}", p.x, p.y);
        }
    };
}

#endif // GRID_H
