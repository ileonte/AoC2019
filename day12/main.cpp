#include <aoc.h>

struct Vec3 {
    int x;
    int y;
    int z;

    void operator+=(const Vec3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
    }

    Vec3& operator=(const Vec3& o) {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }

    friend inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }

    friend inline std::ostream& operator<<(std::ostream& o, const Vec3& v) {
        fmt::print(o, "<x={:3}, y={:3}, z={:3}>", v.x, v.y, v.z);
        return o;
    }

    friend inline bool operator==(const Vec3& v1, const Vec3& v2) {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }
};
namespace std {
    template <> struct hash<Vec3> {
        size_t operator()(const Vec3& v) const {
            return aoc::hash(v.x, v.y, v.z); // ?????
        }
    };
}

struct Planet {
    Vec3 pos;
    Vec3 vel;

    static inline std::optional<Planet> from_string(std::string_view data) {
        auto trimmed = aoc::trim(data);
        if (trimmed.front() != '<') return {};
        if (trimmed.back() != '>') return {};
        trimmed.remove_prefix(1);
        trimmed.remove_suffix(1);

        auto parts = aoc::str_split(trimmed, ',');
        if (parts.size() != 3) return {};

        int x, y, z;
        static constexpr const auto parse_coord = [](char id, std::string_view raw, int& dst) -> bool {
            auto trimmed = aoc::trim(raw);
            if (trimmed.front() != id) return false;
            auto parts = aoc::str_split(trimmed, '=');
            if (parts.size() != 2) return false;
            if (aoc::trim(parts[0]) != std::string_view(&id, 1)) return false;
            auto nstr = aoc::trim(parts[1]);
            auto r = std::from_chars(std::begin(nstr), std::end(nstr), dst);
            if (r.ec != std::errc()) return false;
            return true;
        };

        if (!parse_coord('x', parts[0], x)) return {};
        if (!parse_coord('y', parts[1], y)) return {};
        if (!parse_coord('z', parts[2], z)) return {};

        return Planet{{x, y, z}, {0, 0, 0}};
    }



    inline int potential_energy() const {
        return std::abs(pos.x) + std::abs(pos.y) + std::abs(pos.z);
    }
    inline int kinetic_energy() const {
        return std::abs(vel.x) + std::abs(vel.y) + std::abs(vel.z);
    }
    inline int energy() const {
        return potential_energy() * kinetic_energy();
    }

    Planet& operator=(const Planet& o) {
        pos = o.pos;
        vel = o.vel;
        return *this;
    }

    friend inline std::ostream& operator<<(std::ostream& o, const Planet& p) {
        fmt::print(o, "{{pos={}, vel={}}}", p.pos, p.vel);
        return o;
    }

    friend inline bool operator==(const Planet& p1, const Planet& p2) {
        return p1.pos == p2.pos && p1.vel == p2.vel;
    }
};
namespace std {
    template <> struct hash<Planet> {
        size_t operator()(const Planet& p) const {
            return aoc::hash(p.pos, p.vel);
        }
    };
}

struct PlanetSystem {
    Planet p1, p2, p3, p4;

    static inline std::optional<PlanetSystem> from_stdin(std::istream& in = std::cin) {
        static constexpr const auto do_read = [](std::istream& in, Planet& dst) -> bool {
            std::string in_s{};

            if (!std::getline(in, in_s)) return {};
            if (auto mb = Planet::from_string(in_s); mb) {
                dst = mb.value();
                return true;
            }
            return false;
        };

        PlanetSystem ret{};
        if (!do_read(in, ret.p1)) return {};
        if (!do_read(in, ret.p2)) return {};
        if (!do_read(in, ret.p3)) return {};
        if (!do_read(in, ret.p4)) return {};
        return std::move(ret);
    }

    static inline void apply_gravity(Planet& p1, Planet& p2) {
        if (p1.pos.x != p2.pos.x) {
            if (p1.pos.x < p2.pos.x) {
                p1.vel.x += 1;
                p2.vel.x -= 1;
            } else {
                p1.vel.x -= 1;
                p2.vel.x += 1;
            }
        }
        if (p1.pos.y != p2.pos.y) {
            if (p1.pos.y < p2.pos.y) {
                p1.vel.y += 1;
                p2.vel.y -= 1;
            } else {
                p1.vel.y -= 1;
                p2.vel.y += 1;
            }
        }
        if (p1.pos.z != p2.pos.z) {
            if (p1.pos.z < p2.pos.z) {
                p1.vel.z += 1;
                p2.vel.z -= 1;
            } else {
                p1.vel.z -= 1;
                p2.vel.z += 1;
            }
        }
    }

    static inline void apply_velocity(Planet& p) {
        p.pos += p.vel;
    }

    inline void simulation_step() {
        apply_gravity(p1, p2); apply_gravity(p3, p4);
        apply_gravity(p1, p3); apply_gravity(p2, p4);
        apply_gravity(p1, p4); apply_gravity(p2, p3);

        apply_velocity(p1);
        apply_velocity(p2);
        apply_velocity(p3);
        apply_velocity(p4);
    }

    size_t energy() const {
        return p1.energy() + p2.energy() + p3.energy() + p4.energy();
    }
};
namespace std {
    template <>
    struct hash<PlanetSystem> {
        inline size_t operator()(const PlanetSystem& ps) const {
            return aoc::hash(ps.p1, ps.p2, ps.p3, ps.p4);
        }
    };
}

static inline void part1(const PlanetSystem& in_planets) {
    PlanetSystem planets(in_planets);

    for (size_t iter = 0; iter < 1000; iter++)
        planets.simulation_step();

    fmt::print("{}\n", planets.energy());
}

static inline void part2(const PlanetSystem& in_planets) {
    PlanetSystem planets(in_planets);
}

int main() {
    std::vector<Planet> planets{};
    std::string in{};

    if constexpr (DEBUG) {
        std::stringstream ss(
            "<x=-1, y=0, z=2>\n"
            "<x=2, y=-10, z=-7>\n"
            "<x=4, y=-8, z=8>\n"
            "<x=3, y=5, z=-1>"
        );

        PlanetSystem ps = PlanetSystem::from_stdin(ss).value();
        for (int i = 0; i < 10; i++)
            ps.simulation_step();
        fmt::print("{}\n", ps.energy());
    }

    if (auto mps = PlanetSystem::from_stdin(); mps) {
        part1(mps.value());
        part2(mps.value());
    }

    return 0;
}
