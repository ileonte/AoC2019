#include <computer.h>

int main() {
    if constexpr (DEBUG) {
        const auto test = [](std::string_view code, const std::vector<aoc::computer::memory_value_t>& inputs = {}, size_t mem_size = 1024) {
            aoc::computer computer;
            computer.add_memory_values(code);
            computer.expand_memory(mem_size);
            for (auto v : inputs) computer.add_input(v);
            computer.execute();
            fmt::print("<< {}\n", computer.outputs());
        };

        test("109,1,204,-1,99");
        test("109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99");
        test("1102,34915192,34915192,7,4,7,99,0");
        test("104,1125899906842624,99");
    }

    auto computer = aoc::computer::read_initial_state();

    const auto run_test = [](const aoc::computer& src, aoc::computer::memory_value_t in, size_t mem_size = 32 * 1024) {
        aoc::computer c(src);
        c.expand_memory(mem_size);
        c.add_input(in);
        c.execute();
        fmt::print("{}\n", c.outputs().back());
    };

    run_test(computer, 1);
    run_test(computer, 2);

    return 0;
}
