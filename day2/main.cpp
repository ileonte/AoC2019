#include <aoc.h>

struct program_state {
    std::vector<long> instructions;
    size_t ip;
};

static inline struct program_state read_program() {
    long v{};
    std::vector<long> instructions;
    while (std::cin >> v)
        instructions.push_back(v);
    return {std::move(instructions), 0};
}

int main() {
    aoc::set_input_delimiters({','});
    struct program_state ps = read_program();
    fmt::print("{} {}\n", ps.instructions.size(), ps.instructions);
    return 0;
}
