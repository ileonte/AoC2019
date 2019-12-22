#ifndef GRID_H
#define GRID_H

#include "aoc.h"

namespace aoc::grid {
    struct point {
        int x{-1};
        int y{-1};

        //    inline bool is_valid() const {
        //        return x != -1 && y != -1;
        //    }

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

        inline const auto& start_point() const { return start_point_; }
        inline bool set_start_point(int x, int y) {
            if (!contains(x, y)) return false;
            start_point_ = {x, y};
            return true;
        }
        inline bool set_start_point(point p) { return set_start_point(p.x, p.y); }

        inline const auto& end_point() const { return end_point_; }
        inline bool set_end_point(int x, int y) {
            if (!contains(x, y)) return false;
            end_point_ = {x, y};
            return true;
        }
        inline bool set_end_point(point p) { return set_end_point(p.x, p.y); }

        inline void clear() {
            *this = Grid();
        }

        inline std::optional<std::string> load_from_file(const char* name) {
            char buff[258]{};
            std::vector<std::vector<tile_type>> lines;
            int line_count{1};

            auto fp = std::fopen(name, "rt");
            if (!fp)
                return fmt::format("Failed to open '{}': {}", name, std::strerror(errno));
            defer { std::fclose(fp); };

            while (std::fgets(buff, sizeof(buff) - 1, fp) == buff) {
                std::string_view line_view(buff);
                if (line_view.back() != '\n')
                    return fmt::format("Line {} is too long", line_count);

                while (!line_view.empty() && std::isspace(line_view.front())) line_view.remove_prefix(1);
                while (!line_view.empty() && std::isspace(line_view.back())) line_view.remove_suffix(1);
                if (line_view.empty())
                    return fmt::format("Line {} is empty or contains only white-space");

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
                        case '#': {
                            line.push_back({});
                            break;
                        }
                        case '.': {
                            line.push_back(tile_type::floor);
                            break;
                        }
                        case 'E':
                        case 'G': {
                            /* we'll treat unit positions as empty floor for now */
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

            clear();
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
        std::optional<point> start_point_{};
        std::optional<point> end_point_{};
    };
}

#endif // GRID_H
