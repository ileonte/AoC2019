#include <computer.h>

using phase_setting_array = std::array<aoc::computer::memory_value_t, 5>;

static inline auto part1(const aoc::computer& computer) {
    phase_setting_array phase_settings{0, 1, 2, 3, 4};
    aoc::computer::memory_value_t ret = std::numeric_limits<aoc::computer::memory_value_t>::min();

    do {
        aoc::computer A(computer);
        aoc::computer B(computer);
        aoc::computer C(computer);
        aoc::computer D(computer);
        aoc::computer E(computer);

        A.add_input({phase_settings[0], aoc::computer::memory_value_t(0)}); A.execute();
        B.add_input({phase_settings[1], A.outputs().back()}); B.execute();
        C.add_input({phase_settings[2], B.outputs().back()}); C.execute();
        D.add_input({phase_settings[3], C.outputs().back()}); D.execute();
        E.add_input({phase_settings[4], D.outputs().back()}); E.execute();

        ret = std::max(ret, E.outputs().back());
    } while (std::next_permutation(std::begin(phase_settings), std::end(phase_settings)));

    return ret;
}

static inline auto part2(const aoc::computer& computer) {
    phase_setting_array phase_settings{5, 6, 7, 8, 9};
    aoc::computer::memory_value_t ret = std::numeric_limits<aoc::computer::memory_value_t>::min();
    std::set<aoc::computer::instruction_code> p_ops{aoc::computer::OP_OUT};

    do {
        aoc::computer A(computer); A.add_input(phase_settings[0]);
        aoc::computer B(computer); B.add_input(phase_settings[1]);
        aoc::computer C(computer); C.add_input(phase_settings[2]);
        aoc::computer D(computer); D.add_input(phase_settings[3]);
        aoc::computer E(computer); E.add_input(phase_settings[4]);

        aoc::computer::memory_value_t in_A{0};
        aoc::computer::memory_value_t in_B{0};
        aoc::computer::memory_value_t in_C{0};
        aoc::computer::memory_value_t in_D{0};
        aoc::computer::memory_value_t in_E{0};

        while (!E.is_halted()) {
            A.add_input(in_A); A.execute_with_pause(p_ops); in_B = A.outputs().back();
            B.add_input(in_B); B.execute_with_pause(p_ops); in_C = B.outputs().back();
            C.add_input(in_C); C.execute_with_pause(p_ops); in_D = C.outputs().back();
            D.add_input(in_D); D.execute_with_pause(p_ops); in_E = D.outputs().back();
            E.add_input(in_E); E.execute_with_pause(p_ops); in_A = E.outputs().back();
        }

        ret = std::max(ret, E.outputs().back());
    } while (std::next_permutation(std::begin(phase_settings), std::end(phase_settings)));

    return ret;
}

int main() {
    auto computer = aoc::computer::read_initial_state();

    fmt::print("{}\n", part1(computer));
    fmt::print("{}\n", part2(computer));

    return 0;
}
