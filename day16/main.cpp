#include <aoc.h>

class pattern {
public:
    struct block {
        int val;
        size_t pos;
        size_t count;

        friend inline std::ostream& operator<<(std::ostream& out, block b) {
            fmt::print(out, "{{{}: {} x {}}}", b.pos, b.val, b.count);
            return out;
        }
    };

    pattern() = delete;

    pattern(const std::vector<int>& base_pattern, size_t max_len, size_t repeat_count = 1)
        : base_pattern_(base_pattern)
        , pos_(0)
        , idx_(0)
        , repeat_count_(repeat_count)
        , max_len_(max_len)
        , first_{true}
    {
        if (base_pattern_.empty()) std::abort();
        if (repeat_count_ < 1) std::abort();
        if (max_len_ < 1) std::abort();
    }

    void reset(size_t repeat_count = 0) {
        pos_ = 0;
        idx_ = 0;
        first_ = true;
        if (repeat_count)
            repeat_count_ = repeat_count;
    }

    std::optional<block> next() {
        if (pos_ >= max_len_) return {};

        block ret = {
            base_pattern_[idx_],
            pos_,
            first_ ? repeat_count_ - 1 : repeat_count_
        };
        pos_ += ret.count;
        idx_ += 1;
        if (idx_ >= base_pattern_.size())
            idx_ = 0;

        if (pos_ > max_len_)
            ret.count = max_len_ - ret.pos;

        first_ = false;

        return ret;
    }

    std::optional<block> next_non_zero() {
        while (true) {
            auto mb = next();
            if (!mb) return mb;
            if (mb.value().val) return mb;
        }
        return {};
    }

    static inline std::vector<block> gen(const std::vector<int>& base_pattern, size_t repeat_count, size_t max_len) {
        pattern p(base_pattern, repeat_count, max_len);
        std::vector<block> ret{};

        ret.reserve(max_len / repeat_count + 1);
        while (true)
            if (auto mb = p.next_non_zero(); mb)
                ret.push_back(mb.value());
            else
                break;

        return ret;
    }

private:
    std::vector<int> base_pattern_;
    size_t pos_;
    size_t idx_;
    size_t repeat_count_;
    size_t max_len_;
    bool first_;
};

static inline void part1(std::string_view in, size_t rounds) {
    std::vector<int> in_n(in.size(), 0);
    for (size_t i = 0; i < in.size(); i++) in_n.at(i) = int(in[i] - '0');

    for (size_t i = 0; i < rounds; i++) {
        pattern p({0, 1, 0, -1}, in_n.size());
        for (size_t i = 1; i <= in_n.size(); i++) {
            int sum{0};

            p.reset(i);

            while (true) {
                if (auto mb = p.next_non_zero(); mb) {
                    const auto& b = mb.value();
                    int tmp = b.val * std::accumulate(&in_n[b.pos], &in_n[b.pos + b.count], 0);
                    sum += tmp;
                } else {
                    break;
                }
            }
            in_n[i - 1] = std::abs(sum) % 10;
        }
    }

    fmt::print("{}\n", fmt::join(&in_n[0], &in_n[8], ""));
}

template <typename T>
static inline T num(T t) {
    return t;
}
template <typename T, typename... Ts>
static inline T num(T t, Ts... ts) {
    return t * aoc::cpow(10, T(sizeof...(Ts))) + num(ts...);
}

static inline void part2(std::string_view in, size_t rounds, size_t multiplier = 10000) {
    std::vector<int> in_n(in.size(), 0);
    for (size_t i = 0; i < in.size(); i++) in_n.at(i) = int(in[i] - '0');

    size_t total_input_size{in.size() * multiplier};
    size_t global_offset = num(in_n[0], in_n[1], in_n[2], in_n[3], in_n[4], in_n[5], in_n[6]);
    size_t global_remaining = total_input_size - global_offset;
    size_t buffer_count = global_remaining / in_n.size();
    size_t buffer_remaining = global_remaining % in_n.size();

    std::vector<int> in_d(global_remaining, 0);
    if (buffer_remaining)
        std::copy(&in_n[in_n.size() - buffer_remaining], &in_n[in_n.size()], &in_d[0]);
    for (size_t i = 0; i < buffer_count; i++)
        std::copy(in_n.begin(), in_n.end(), &in_d[buffer_remaining + i * in_n.size()]);

    for (size_t round = 0; round < rounds; round++) {
        int sum{0};
        for (auto it = in_d.rbegin(); it != in_d.rend(); it++) {
            sum += *it;
            *it = sum % 10;
        }
    }

    fmt::print("{}\n", fmt::join(&in_d[0], &in_d[8], ""));
}

int main() {
    if constexpr (DEBUG){
        pattern p({0, 1, 0, -1}, 8, 1);
        while (true) {
            if (auto mb = p.next_non_zero(); mb)
                fmt::print("{} ", mb.value());
            else
                break;
        }
        fmt::print("\n");

        part1("12345678", 4);
        part1("80871224585914546619083218645595", 100);

        part2("03036732577212944063491565474664", 100);
    }

    std::string in{};
    std::getline(std::cin, in);

    part1(in, 100);
    part2(in, 100);

    return 0;
}
