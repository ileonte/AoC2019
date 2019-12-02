#include <aoc.h>

using memory_value_t = long;

enum instruction_codes : memory_value_t {
    OP_ADD = 1,
    OP_MUL = 2,
    OP_HLT = 99
};

struct computer_state {
    std::vector<memory_value_t> memory;
    size_t ip;
};

static inline struct computer_state read_initial_state() {
    memory_value_t v{};
    std::vector<memory_value_t> memory;
    while (std::scanf("%ld,", &v) > 0)
        memory.push_back(v);
    return {std::move(memory), 0};
}

static inline void execute_program(struct computer_state& state) {
    auto& memory = state.memory;
    while (state.ip < memory.size()) {
        switch (memory.at(state.ip)) {
            case OP_ADD: {
                auto v1 = memory[size_t(memory[state.ip + 1])];
                auto v2 = memory[size_t(memory[state.ip + 2])];
                memory[size_t(memory[state.ip + 3])] = v1 + v2;
                state.ip += 4;
                break;
            }
            case OP_MUL: {
                auto v1 = memory[size_t(memory[state.ip + 1])];
                auto v2 = memory[size_t(memory[state.ip + 2])];
                memory[size_t(memory[state.ip + 3])] = v1 * v2;
                state.ip += 4;
                break;
            }
            case OP_HLT: {
                state.ip = memory.size() + 1;
                break;
            }
        }
    }
}

static inline memory_value_t compute(const struct computer_state& state,
                                     memory_value_t noun,
                                     memory_value_t verb) {
    struct computer_state copy_state = state;
    copy_state.memory[1] = noun;
    copy_state.memory[2] = verb;
    execute_program(copy_state);
    return copy_state.memory[0];
}

int main() {
    if constexpr (DEBUG) {
        auto test = [](const std::vector<memory_value_t>& memory) {
            computer_state state{memory, 0};
            execute_program(state);
            fmt::print("{}\n", state.memory);
        };
        test({1, 0, 0, 0, 99});
        test({2, 3, 0, 3, 99});
        test({2, 4, 4, 5, 99, 0});
        test({1, 1, 1, 4, 99, 5, 6, 0, 99});
    }
    auto program = read_initial_state();

    fmt::print("{}\n", compute(program, 12, 2));

    for (memory_value_t noun = 0; noun < 100; noun++) {
        for (memory_value_t verb = 0; verb < 100; verb++) {
            if (compute(program, noun, verb) == 19690720) {
                fmt::print("{}\n", 100 * noun + verb);
                return 0;
            }
        }
    }

    return 0;
}
