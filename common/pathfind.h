#pragma once

#include "grid.h"

namespace aoc::pathfinding {
    class AStar {
    public:
        using Grid = aoc::grid::Grid;
        using tile_type = aoc::grid::tile_type;
        using point = aoc::grid::point;

        static constexpr const inline bool can_move_diagonally = false;

        struct node_light {
            tile_type type{tile_type::wall};
            point pos{};
            point parent{-1, -1};
            int g_cost{0};
            int h_cost{0};

            inline int f_cost() const { return g_cost + h_cost; }
        };
        struct node : public node_light {
            std::vector<point> special_neighbours{};
        };

        inline bool is_empty() const { return board_.empty(); };
        inline bool is_done() const { return !is_empty() && !path_.empty(); }
        inline int width() const { return is_empty() ? 0 : board_.front().size(); }
        inline int height() const { return is_empty() ? 0 : board_.size(); }
        inline void clear() {
            board_.clear();
            open_.clear();
            closed_.clear();
            path_.clear();
        }

        inline bool contains(point p) const {
            return !is_empty() && (std::clamp(p.x, 0, width() - 1) == p.x && std::clamp(p.y, 0, height() - 1) == p.y);
        }
        inline bool contains(int x, int y) const {
            return contains({x, y});
        }

        inline bool is_blocked(point p) const {
            if (!contains(p)) return true;
            return board_[p.y][p.x].type != tile_type::floor;
        }
        inline bool is_blocked(int x, int y) const { return is_blocked({x, y}); }

        inline node& at(point p) {
            assert(contains(p));
            return board_[p.y][p.x];
        }
        inline const node& at(point p) const {
            assert(contains(p));
            return board_[p.y][p.x];
        }

        inline bool reset() {
            open_.clear();
            closed_.clear();
            path_.clear();

            for (auto& line : board_) {
                for (auto& node : line) {
                    node.g_cost = std::numeric_limits<int>::max();
                    node.h_cost = 0;
                    node.parent = {-1, -1};
                }
            }

            return true;
        }

        inline bool init(const Grid& grid) {
            if (grid.is_empty())
                return false;

            clear();

            for (size_t y = 0; y < grid.raw_grid().size(); y++) {
                const auto& raw_line = grid.raw_grid()[y];
                std::vector<node> line(raw_line.size(), node{});
                for (size_t x = 0; x < raw_line.size(); x++) {
                    line[x].type = raw_line[x];
                    line[x].pos.x = x;
                    line[x].pos.y = y;
                    line[x].special_neighbours = grid.special_neighours(line[x].pos);
                }
                board_.push_back(std::move(line));
            }

            if (!reset()) {
                clear();
                return false;
            }
            return true;
        }

        inline void step(const point& start, const point& end) {
            if (is_done()) return;

            if (open_.empty()) {
                if (is_blocked(start) || is_blocked(end))
                    return;

                open_.push_back(start);
                at(start).g_cost = 0;
                at(end).g_cost = 0;
                return;
            }

            auto current_point = get_min_open();
            closed_.push_back(current_point);

            if (current_point == end) {
                point it = end;
                while (!(it == start)) {
                    path_.push_back(it);
                    it = at(it).parent;
                }
                return;
            }

            auto& current_node = at(current_point);
            std::vector<node_light> neighbours{};
            init_neighbours(current_node, neighbours, end);
            for (auto& neighbour : neighbours) {
                if (node_is_closed(neighbour.pos)) continue;

                node& current_neighbor = at(neighbour.pos);
                bool is_open = node_is_open(neighbour.pos);
                bool better_path = (current_neighbor.g_cost != std::numeric_limits<int>::max());
                better_path = better_path && (neighbour.f_cost() < current_neighbor.f_cost());
                if (!is_open || better_path) {
                    current_neighbor.g_cost = neighbour.g_cost;
                    current_neighbor.h_cost = neighbour.h_cost;
                    current_neighbor.parent = neighbour.parent;

                    if (!is_open) open_.push_back(neighbour.pos);
                }
            }
        }

        inline const auto& find_path(const point& start, const point& end) {
            reset();

            while (!is_done())
                step(start, end);

            return path_;
        }

        inline const auto& open_set() const { return open_; }
        inline const auto& closed_set() const { return closed_; }
        inline const auto& board() const { return board_; }

        inline const auto& path() const { return path_; }

    private:
        static inline int shortest_path(point p1, point p2) {
            int dx = std::abs(p1.x - p2.x);
            int dy = std::abs(p1.y - p2.y);

            if (p1.x == p2.x) return dy * 10;
            if (p1.y == p2.y) return dx * 10;
            if (dx == dy) return dx * 14;

            int dd = std::abs(dx - dy);
            return dd * 10 + std::min(dx, dy) * 14;
        }

        static inline int manhattan(point p1, point p2) {
            return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
        }

        inline point get_min_open() {
            auto it = std::min_element(open_.begin(), open_.end(), [&](point p1, point p2) -> bool {
                const auto& n1 = at(p1);
                const auto& n2 = at(p2);
                if (n1.f_cost() < n2.f_cost()) return true;
                if (n1.f_cost() == n2.f_cost()) return n1.h_cost < n2.h_cost;
                return false;
            });
            point min_n = *it;
            auto lit = std::prev(open_.end());
            std::swap(*it, *lit);
            open_.pop_back();

            return min_n;
        }

        inline void init_neighbours(node& n, std::vector<node_light>& neighbors, const point& end) {
            static constexpr const auto add_neighbor = [](const AStar* astar, const node& parent_node, point neighbor_point,
                    int g_inc, std::vector<node_light>& neighbors, const point& end) {
                if (!astar->is_blocked(neighbor_point)) {
                    if constexpr (can_move_diagonally) {
                        node_light new_node {
                            /* type   */ parent_node.type,
                            /* pos    */ neighbor_point,
                            /* parent */ parent_node.pos,
                            /* g_cost */ parent_node.g_cost + g_inc,
                            /* h_cost */ shortest_path(neighbor_point, end)
                        };
                        neighbors.push_back(new_node);
                    } else {
                        node_light new_node {
                            /* type   */ parent_node.type,
                            /* pos    */ neighbor_point,
                            /* parent */ parent_node.pos,
                            /* g_cost */ parent_node.g_cost + g_inc,
                            /* h_cost */ manhattan(neighbor_point, end)
                        };
                        neighbors.push_back(new_node);
                    }
                }
            };

            neighbors.clear();

            if constexpr (can_move_diagonally) {
                neighbors.reserve(8 + n.special_neighbours.size());
                add_neighbor(this, n, {n.pos.x - 1, n.pos.y - 1}, 14, neighbors, end);
                add_neighbor(this, n, {n.pos.x    , n.pos.y - 1}, 10, neighbors, end);
                add_neighbor(this, n, {n.pos.x + 1, n.pos.y - 1}, 14, neighbors, end);
                add_neighbor(this, n, {n.pos.x - 1, n.pos.y    }, 10, neighbors, end);
                add_neighbor(this, n, {n.pos.x + 1, n.pos.y    }, 10, neighbors, end);
                add_neighbor(this, n, {n.pos.x - 1, n.pos.y + 1}, 14, neighbors, end);
                add_neighbor(this, n, {n.pos.x    , n.pos.y + 1}, 10, neighbors, end);
                add_neighbor(this, n, {n.pos.x + 1, n.pos.y + 1}, 14, neighbors, end);
            } else {
                neighbors.reserve(4 + n.special_neighbours.size());
                add_neighbor(this, n, {n.pos.x    , n.pos.y - 1},  1, neighbors, end);
                add_neighbor(this, n, {n.pos.x - 1, n.pos.y    },  1, neighbors, end);
                add_neighbor(this, n, {n.pos.x + 1, n.pos.y    },  1, neighbors, end);
                add_neighbor(this, n, {n.pos.x    , n.pos.y + 1},  1, neighbors, end);
            }

            for (auto& np : n.special_neighbours)
                add_neighbor(this, n, np, 0, neighbors, end);
        }

        inline bool node_is_closed(point p) const {
            return std::find_if(closed_.begin(), closed_.end(), [&](const point& c) { return p == c; }) != closed_.end();
        }

        inline bool node_is_open(point p) const {
            return std::find_if(open_.begin(), open_.end(), [&](const point& c) { return p == c; }) != open_.end();
        }

        std::vector<std::vector<node>> board_{};
        std::vector<point> open_{};
        std::vector<point> closed_{};
        std::vector<point> path_{};
    };
}
