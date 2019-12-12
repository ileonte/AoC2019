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

    friend inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }

    friend inline std::ostream& operator<<(std::ostream& o, const Vec3& v) {
        fmt::print(o, "<x={:3}, y={:3}, z={:3}>", v.x, v.y, v.z);
        return o;
    }
};

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

    friend inline std::ostream& operator<<(std::ostream& o, const Planet& p) {
        fmt::print(o, "{{pos={}, vel={}}}", p.pos, p.vel);
        return o;
    }
};

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

static inline void simulation_step(std::vector<Planet>& planets) {
    apply_gravity(planets.at(0), planets.at(1));
    apply_gravity(planets.at(2), planets.at(3));

    apply_gravity(planets.at(0), planets.at(2));
    apply_gravity(planets.at(1), planets.at(3));

    apply_gravity(planets.at(0), planets.at(3));
    apply_gravity(planets.at(1), planets.at(2));

    for (auto& p : planets) apply_velocity(p);
}

static inline void part1(const std::vector<Planet>& in_planets) {
    std::vector<Planet> planets(in_planets);

    for (size_t iter = 0; iter < 1000; iter++)
        simulation_step(planets);

    int result = std::accumulate(std::begin(planets), std::end(planets), 0, [](int s, const Planet& p) -> int {
        return s + p.energy();
    });
    fmt::print("{}\n", result);
}

int main() {
    std::vector<Planet> planets{};
    std::string in{};

    if constexpr (DEBUG) {
        planets.push_back(Planet::from_string("<x=-1, y=0, z=2>").value());
        planets.push_back(Planet::from_string("<x=2, y=-10, z=-7>").value());
        planets.push_back(Planet::from_string("<x=4, y=-8, z=8>").value());
        planets.push_back(Planet::from_string("<x=3, y=5, z=-1>").value());

        for (int i = 0; i < 10; i++)
            simulation_step(planets);

        fmt::print("{}\n", std::accumulate(std::begin(planets), std::end(planets), 0, [](int s, const Planet& p) -> int {
            return s + p.energy();
        }));

        planets.clear();
    }

    while (std::getline(std::cin, in)) {
        auto maybe_planet = Planet::from_string(in);
        if (maybe_planet) planets.push_back(maybe_planet.value());
    }

    if (planets.size() != 4) {
        fmt::print(::stderr, "Not 4 planets!?\n");
        return 1;
    }

    part1(planets);

    return 0;
}
