#pragma once

#include "aoc.h"

#define CF_HALTED 0x1

namespace aoc {
    namespace detail {
        template <typename cb_t, size_t max_entries>
        class instruction_callback_storage {
        public:
            struct init_s {
                size_t opcode;
                cb_t cb;
            };

            inline constexpr instruction_callback_storage() noexcept = default;
            inline constexpr instruction_callback_storage(cb_t def) noexcept {
                for (auto& ref : callbacks_)
                    ref = def;
            }
            template <size_t N>
            inline constexpr instruction_callback_storage(cb_t def, init_s const (&vals)[N]) noexcept
                : instruction_callback_storage(def)
            {
                for (const auto& v : vals)
                    callbacks_[v.opcode] = v.cb;
            }

            inline constexpr cb_t operator[](int code) {
                assert(size_t(code) < max_entries);
                return callbacks_[code];
            }
            inline constexpr cb_t operator[](int code) const {
                assert(size_t(code) < max_entries);
                return callbacks_[code];
            }

        private:
            cb_t callbacks_[max_entries]{};
        };
    }

    class computer {
    public:
        using memory_value_t = long;

        computer() = default;
        computer(computer&&) = default;
        computer(const computer& other) = default;

        static inline computer read_initial_state() {
            computer ret{};
            std::string buff;
            std::vector<memory_value_t> memory;
            while (std::getline(std::cin, buff)) {
                if (!ret.add_memory_values(buff))
                    break;
            }
            return ret;
        }

        const auto& memory() const {
            return memory_;
        }
        bool add_memory_value(std::string_view buff) {
            memory_value_t v{};
            std::string_view sv = aoc::trim(buff);
            if (sv.empty())
                return true;
            auto r = std::from_chars(sv.cbegin(), sv.cend(), v);
            if (r.ec != std::errc())
                return false;
            memory_.push_back(v);
            return true;
        }
        bool add_memory_values(std::string_view buff) {
            for (const auto& v : aoc::str_split(buff, ',')) {
                if (!add_memory_value(v))
                    return false;
            }
            return true;
        }
        void expand_memory(size_t size, memory_value_t initial_value = 0) {
            if (size > memory_.size()) {
                memory_.reserve(size);
                while (memory_.size() < memory_.capacity())
                    memory_.push_back(initial_value);
            }
        }

        void execute() {
            clear_flags(CF_HALTED);
            while (!has_flags(CF_HALTED)) {
                auto raw = memory_.at(size_t(ip()));
                auto code = raw % 100;
                auto mode = raw / 100;
                instruction_callbacks[int(code)](*this, mode);
            }
        }

        void clear() {
            memory_.clear();
            registers_.fill(0);
            inputs_.clear();
            outputs_.clear();
            flags_ = 0;
        }

        const auto& inputs() const {
            return inputs_;
        }
        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
        void add_input(T v) {
            inputs_.push_back(memory_value_t(v));
        }
        template <typename T, size_t N, typename = typename std::enable_if<std::is_integral<T>::value>::type>
        void add_input(T const (&va)[N]) {
            for (const auto& v : va)
                inputs_.push_back(memory_value_t(v));
        }
        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
        void set_input(T v) {
            inputs_.clear();
            inputs_.push_back(memory_value_t(v));
        }
        template <typename T, size_t N, typename = typename std::enable_if<std::is_integral<T>::value>::type>
        void set_input(T const (&va)[N]) {
            inputs_.clear();
            for (const auto& v : va)
                inputs_.push_back(memory_value_t(v));
        }
        void clear_input() {
            inputs_.clear();
        }

        const auto& outputs() const {
            return outputs_;
        }
        void clear_output() {
            outputs_.clear();
        }
        auto get_output() {
            auto ret = std::optional<memory_value_t>(outputs_.size() ? outputs_.front() : std::optional<memory_value_t>());
            if (outputs_.size()) outputs_.pop_front();
            return ret;
        }

        auto flags() const {
            return flags_;
        }
        auto set_flags(memory_value_t flags) {
            flags_ |= flags;
        }
        void clear_flags(memory_value_t flags) {
            flags_ &= ~flags;
        }
        bool has_flags(memory_value_t flags) const {
            return (flags_ & flags) == flags;
        }

    private:
        enum instruction_code : memory_value_t {
            OP_ADD = 1,
            OP_MUL = 2,
            OP_IN  = 3,
            OP_OUT = 4,
            OP_JNZ = 5,
            OP_JZ  = 6,
            OP_LT  = 7,
            OP_EQ  = 8,
            OP_SRB = 9,   // Set Relative Base
            OP_HLT = 99,
            OP_INVAL,
        };

        enum register_code : memory_value_t {
            RC_IP,
            RC_RELBASE,
            RC_MAX_
        };

        enum addressing_mode : memory_value_t {
            AM_POSITION = 0,
            AM_IMMEDIATE = 1,
            AM_RELBASE = 2,
            AM_MAX_,
        };

        struct decoded_instruction_t {
            instruction_code code{OP_INVAL};
            memory_value_t modes{0};
        };

        template <register_code reg>
        inline memory_value_t& reg_ref() {
            return registers_.at(reg);
        }

        inline memory_value_t& ip() {
            return reg_ref<RC_IP>();
        }

        inline memory_value_t& mem_ref(memory_value_t address, addressing_mode mode = AM_POSITION) {
            switch (mode) {
                case AM_POSITION: return memory_.at(size_t(memory_.at(size_t(address))));
                case AM_IMMEDIATE: return memory_.at(size_t(address));
                case AM_RELBASE: return memory_.at(size_t(memory_.at(size_t(address)) + reg_ref<RC_RELBASE>()));
                default: std::abort();
            }
        }

        template <memory_value_t param, memory_value_t max_params>
        static inline addressing_mode mode_for(memory_value_t modes) {
            constexpr const auto actual_param = std::clamp<memory_value_t>(param, 1, max_params);
            constexpr const auto div = cpow<memory_value_t>(10, actual_param - 1);
            return addressing_mode((modes / div) % 10);
        }

        [[noreturn]] static inline void icb_invalid_instruction(computer& c, memory_value_t) {
            fmt::print(::stderr, "INVALID INSTRUCTION at IP={}:\n{}\n", c.ip(), c.memory());
            std::abort();
        }
        static inline void icb_add(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 3>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 3>(modes));
            auto& out = c.mem_ref(c.ip() + 3, mode_for<3, 3>(modes));
            out = in1 + in2;
            c.ip() += 4;
        }
        static inline void icb_mul(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 3>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 3>(modes));
            auto& out = c.mem_ref(c.ip() + 3, mode_for<3, 3>(modes));
            out = in1 * in2;
            c.ip() += 4;
        }
        static inline void icb_in(computer& c, memory_value_t modes) {
            auto& out = c.mem_ref(c.ip() + 1, mode_for<1, 1>(modes));
            out = c.inputs_.front();
            c.inputs_.pop_front();
            c.ip() += 2;
        }
        static inline void icb_out(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 1>(modes));
            c.outputs_.push_back(in1);
            c.ip() += 2;
        }
        static inline void icb_jnz(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 2>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 2>(modes));
            if (in1)
                c.ip() = in2;
            else
                c.ip() += 3;
        }
        static inline void icb_jz(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 2>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 2>(modes));
            if (!in1)
                c.ip() = in2;
            else
                c.ip() += 3;
        }
        static inline void icb_lt(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 3>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 3>(modes));
            auto& out = c.mem_ref(c.ip() + 3, mode_for<3, 3>(modes));
            out = in1 < in2 ? 1 : 0;
            c.ip() += 4;
        }
        static inline void icb_eq(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 3>(modes));
            auto& in2 = c.mem_ref(c.ip() + 2, mode_for<2, 3>(modes));
            auto& out = c.mem_ref(c.ip() + 3, mode_for<3, 3>(modes));
            out = in1 == in2 ? 1 : 0;
            c.ip() += 4;
        }
        static inline void icb_srb(computer& c, memory_value_t modes) {
            auto& in1 = c.mem_ref(c.ip() + 1, mode_for<1, 1>(modes));
            c.reg_ref<RC_RELBASE>() += in1;
            c.ip() += 2;
        }
        static inline void icb_hlt(computer& c, memory_value_t) {
            c.set_flags(CF_HALTED);
            c.ip() += 1;
        }

        using instruction_callback_t = void (*)(computer&, memory_value_t);
        using instruction_callbacks_t = detail::instruction_callback_storage<instruction_callback_t, OP_INVAL>;
        static inline constexpr const auto instruction_callbacks = instruction_callbacks_t(
                    &computer::icb_invalid_instruction, {
                        {OP_ADD, &computer::icb_add},
                        {OP_MUL, &computer::icb_mul},
                        {OP_IN,  &computer::icb_in},
                        {OP_OUT, &computer::icb_out},
                        {OP_JNZ, &computer::icb_jnz},
                        {OP_JZ,  &computer::icb_jz},
                        {OP_LT,  &computer::icb_lt},
                        {OP_EQ,  &computer::icb_eq},
                        {OP_SRB, &computer::icb_srb},
                        {OP_HLT, &computer::icb_hlt},
                    });

        std::array<memory_value_t, register_code::RC_MAX_> registers_{};
        std::deque<memory_value_t> inputs_{};
        std::deque<memory_value_t> outputs_{};
        memory_value_t flags_{0};
        std::vector<memory_value_t> memory_{};
    };
}
