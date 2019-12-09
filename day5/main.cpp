#include <aoc.h>
#include <computer.h>

int main() {
    if constexpr (DEBUG) {
        const auto test = [](std::string_view code, const std::vector<aoc::computer::memory_value_t>& inputs = {}) {
            aoc::computer computer;
            computer.add_memory_values(code);
            for (auto v : inputs) computer.add_input(v);
            computer.execute();
            fmt::print("<< {}\n", computer.outputs());
        };

        std::string_view program("3,21,1008,21,8,20,1005,20,22,107,8,21,20,1006,20,31,"
                                 "1106,0,36,98,0,0,1002,21,125,20,4,20,1105,1,46,104,"
                                 "999,1105,1,46,1101,1000,1,20,4,20,1105,1,46,98,99");
        test(program, {7});
        test(program, {8});
        test(program, {9});
    }

    auto computer = aoc::computer::read_initial_state();

    const auto run_test = [](const aoc::computer& src, auto in) {
        aoc::computer c(src);
        c.add_input(in);
        c.execute();
        fmt::print("{}\n", c.outputs().back());
    };

    run_test(computer, 1);
    run_test(computer, 5);

    return 0;
}
