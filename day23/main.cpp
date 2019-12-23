#include <computer.h>
#include <queue>

using int_type = aoc::computer::memory_value_t;

static std::array<aoc::computer, 50> computers{};

struct packet {
    std::optional<int_type> addr;
    std::optional<int_type> x;
    std::optional<int_type> y;

    inline bool is_complete() const {
        return addr.has_value() && x.has_value() && y.has_value();
    }

    inline void reset() {
        addr.reset();
        x.reset();
        y.reset();
    }

    inline void add_value(int_type val) {
        if (!addr.has_value()) {
            addr = val;
            return;
        }
        if (!x.has_value()) {
            x = val;
            return;
        }
        if (!y.has_value()) {
            y = val;
            return;
        }
    }

    inline friend std::ostream& operator<<(std::ostream& out, const packet& p) {
        auto s = fmt::format("PACKET(addr={:>2}, x={}, y={})",
                             p.addr.has_value() ? std::to_string(p.addr.value()) : "(missing)",
                             p.x.has_value() ? std::to_string(p.x.value()) : "(missing)",
                             p.y.has_value() ? std::to_string(p.y.value()) : "(missing)");
        return out << s;
    }
};

static inline void run_network(const aoc::computer& nic) {
    for (int_type addr = 0; addr < int_type(computers.size()); addr++) {
        auto& c = computers.at(addr);
        c = nic;
        c.expand_memory(32 * 1024);
        c.set_default_input(-1);
        c.add_input(addr);
    }

    std::array<packet, computers.size()> packets{};
    std::queue<packet> queue{};
    packet nat{};
    packet last_nat{};
    bool part1_done{false};
    bool part2_done{false};

    while (!part1_done || !part2_done) {
        int_type writes{0};

        for (int_type src = 0; src < int_type(computers.size()); src++) {
            auto& c = computers.at(src);
            c.single_step();
            if (c.outputs().size()) {
                auto val = c.get_output().value();
                auto& p = packets.at(src);
                if (!p.addr.has_value()) {
                    p.add_value(val);
                    writes += 1;
                    continue;
                }

                if (!p.x.has_value()) {
                    p.add_value(val);
                    writes += 1;
                    assert(p.x.has_value());
                    continue;
                }

                assert(!p.is_complete());
                assert(!p.y.has_value());
                p.add_value(val);
                writes += 1;
                assert(p.is_complete());
                queue.push(p);
                p.reset();
            }
        }

        bool inputs_empty = std::accumulate(computers.begin(), computers.end(), true, [](bool v, const aoc::computer& c) -> bool {
            return v && c.inputs().empty();
        });
        bool idle{inputs_empty && (writes == 0)};

        if (idle) {
            if (!nat.is_complete())
                continue;
            if (last_nat.is_complete() && last_nat.y.value() == nat.y.value()) {
                fmt::print("{}\n", nat.y.value());
                part2_done = true;
                continue;
            }
            computers.at(0).add_input({nat.x.value(), nat.y.value()});
            last_nat = nat;
            nat.reset();
        } else {
            while (queue.size()) {
                auto& p = queue.front();
                assert(p.is_complete());
                auto dst = p.addr.value();
                if (dst >= 0 && dst <= int_type(computers.size())) {
                    computers.at(dst).add_input({p.x.value(), p.y.value()});
                } else {
                    assert(dst == 255);
                    if (!part1_done) {
                        fmt::print("{}\n", p.y.value());
                        part1_done = true;
                    }
                    nat = p;
                }
                queue.pop();
            }
        }
    }
}

int main() {
    auto nic = aoc::computer::read_initial_state();

    run_network(nic);

    return 0;
}
