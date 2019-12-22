#pragma once

#include "grid.h"

namespace aoc::pathfinding {
    using tile_type = aoc::grid::tile_type;
    using point = aoc::grid::point;

    class AStar {
    public:
        struct node {
            tile_type type{tile_type::wall};
            point pos{};
            point parent{-1, -1};
            int g_cost{0};
            int h_cost{0};

            inline int f_cost() const { return g_cost + h_cost; }
        };

        inline bool is_empty() const { return board_.empty(); };
        inline bool is_done() const { return !is_empty() && !path_.empty(); }
        inline bool is_reachable() const {
            if (!is_done()) return true;
            return path_.front() == end_;
        }
        inline int width() const { return is_empty() ? 0 : board_.front().size(); }
        inline int height() const { return is_empty() ? 0 : board_.size(); }
        inline void clear() {
            board_.clear();
            open_.clear();
            closed_.clear();
            start_ = {};
            end_ = {};
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

        inline bool reset(point start, point end) {
            if (is_blocked(start) || is_blocked(end))
                return false;

            start_ = start;
            end_ = end;

            open_.clear();
            closed_.clear();
            path_.clear();

            open_.push_back(start_);

            for (auto& line : board_) {
                for (auto& node : line) {
                    node.g_cost = std::numeric_limits<int>::max();
                    node.h_cost = 0;
                }
            }
            at(start_).g_cost = 0;
            at(end_).g_cost = 0;

            return true;
        }

        inline bool init(const aoc::grid::Grid& grid) {
            if (grid.is_empty() || !grid.start_point().has_value() || !grid.end_point().has_value())
                return false;

            clear();

            for (size_t y = 0; y < grid.raw_grid().size(); y++) {
                const auto& raw_line = grid.raw_grid()[y];
                std::vector<node> line(raw_line.size(), node{});
                for (size_t x = 0; x < raw_line.size(); x++) {
                    line[x] = {
                        /* type   */ raw_line[x],
                        /* pos    */ {(int)x, (int)y},
                        /* parent */ {-1, -1},
                        /* g_cost */ std::numeric_limits<int>::max(),
                        /* h_cost */ 0
                    };
                }
                board_.push_back(std::move(line));
            }

            if (!reset(grid.start_point().value(), grid.end_point().value())) {
                clear();
                return false;
            }
            return true;
        }

        inline void step() {
            if (is_done()) return;

            if (open_.empty()) {
                path_.push_back(start_);
                return;
            }

            auto current_point = get_min_open();
            closed_.push_back(current_point);

            if (current_point == end_) {
                point it = end_;
                while (!(it == start_)) {
                    path_.push_back(it);
                    it = at(it).parent;
                }
                return;
            }

            std::vector<node> neighbors{};
            neighbors_of(current_point, neighbors);
            for (auto neighbor : neighbors) {
                if (node_is_closed(neighbor.pos)) continue;

                node& current_neighbor = at(neighbor.pos);
                bool is_open = node_is_open(neighbor.pos);
                bool better_path = (current_neighbor.g_cost != std::numeric_limits<int>::max());
                better_path = better_path && (neighbor.f_cost() < current_neighbor.f_cost());
                if (!is_open || better_path) {
                    current_neighbor.g_cost = neighbor.g_cost;
                    current_neighbor.h_cost = neighbor.h_cost;
                    current_neighbor.parent = neighbor.parent;

                    if (!is_open) open_.push_back(neighbor.pos);
                }
            }
        }

        inline const auto& open_set() const { return open_; }
        inline const auto& closed_set() const { return closed_; }
        inline const auto& board() const { return board_; }

        inline const auto& path() const { return path_; }

        inline point start() const { return start_; }
        inline point end() const { return end_; }

        inline bool can_move_diagonally() const { return can_move_diagonally_; }
        inline void can_move_diagonally(bool yesno) { can_move_diagonally_ = yesno; }

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

        inline void neighbors_of(point p, std::vector<node>& neighbors) {
            static const auto add_neighbor = [](AStar* astar, point parent_point, point neighbor_point,
                    int g_inc, std::vector<node>& neighbors) {
                if (!astar->is_blocked(neighbor_point)) {
                    const node& parent_node = astar->at(parent_point);
                    node new_node {
                        /* type   */ parent_node.type,
                        /* pos    */ neighbor_point,
                        /* parent */ parent_point,
                        /* g_cost */ parent_node.g_cost + g_inc,
                        /* h_cost */ astar->can_move_diagonally() ? shortest_path(neighbor_point, astar->end()) : manhattan(neighbor_point, astar->end())
                    };
                    neighbors.push_back(new_node);
                }
            };

            neighbors.reserve(8);
            neighbors.clear();

            if (can_move_diagonally_) {
                add_neighbor(this, p, {p.x - 1, p.y - 1}, 14, neighbors);
                add_neighbor(this, p, {p.x    , p.y - 1}, 10, neighbors);
                add_neighbor(this, p, {p.x + 1, p.y - 1}, 14, neighbors);
                add_neighbor(this, p, {p.x - 1, p.y    }, 10, neighbors);
                add_neighbor(this, p, {p.x + 1, p.y    }, 10, neighbors);
                add_neighbor(this, p, {p.x - 1, p.y + 1}, 14, neighbors);
                add_neighbor(this, p, {p.x    , p.y + 1}, 10, neighbors);
                add_neighbor(this, p, {p.x + 1, p.y + 1}, 14, neighbors);
            } else {
                add_neighbor(this, p, {p.x    , p.y - 1},  1, neighbors);
                add_neighbor(this, p, {p.x - 1, p.y    },  1, neighbors);
                add_neighbor(this, p, {p.x + 1, p.y    },  1, neighbors);
                add_neighbor(this, p, {p.x    , p.y + 1},  1, neighbors);
            }
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
        bool can_move_diagonally_{false};
        point start_{};
        point end_{};
    };
}
