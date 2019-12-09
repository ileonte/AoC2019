#include <aoc.h>

static constexpr const int image_width = 25;
static constexpr const int image_height = 6;
static constexpr size_t image_size = image_width * image_height;

static inline std::vector<std::string_view> split_layers(std::string_view data) {
    std::vector<std::string_view> ret{};
    data = aoc::trim(data);
    for (size_t i = 0; i < data.size() / image_size; i++) {
        ret.push_back(aoc::substr(data, i * image_size, image_size));
        assert(ret.back().size() == image_size);
    }
    return ret;
}

static inline std::tuple<int, int, int> count_012(const std::string_view& layer) {
    int c0{0};
    int c1{0};
    int c2{0};
    for (auto c : layer) {
        switch (c) {
            case '0': {
                c0++;
                break;
            }
            case '1': {
                c1++;
                break;
            }
            case '2': {
                c2++;
                break;
            }
            default: continue;
        }
    }
    return {c0, c1, c2};
}

static inline void merge_layer(std::string& merged, const std::string_view& layer) {
    assert(merged.size() == layer.size());
    for (size_t i = 0; i < layer.size(); i++) {
        auto& c = merged.at(i);
        if (c != ' ') continue;
        if (layer.at(i) == '0') c = '#';
    }
    fmt::print("");
}

static inline void print_image(const std::vector<std::string_view>& layers) {
    for (size_t row = 0; row < image_height; row++) {
        for (size_t col = 0; col < image_width; col++) {
            for (const auto& layer : layers) {
                const auto& color = layer.at(row * image_width + col);
                if (color == '2') continue;
                fmt::print("{}", color == '1' ? '#' : '.');
                break;
            }
        }
        fmt::print("\n");
    }
}

int main() {
    std::string in{};
    std::string merged(image_size, ' ');
    std::getline(std::cin, in);

    auto layers = split_layers(in);

    int answer_part1 = 0;
    int min0 = std::numeric_limits<int>::max();
    for (const auto& layer : layers) {
        auto [c0, c1, c2] = count_012(layer);
        if (std::min(c0, min0) == c0) {
            min0 = c0;
            answer_part1 = c1 * c2;
        }
        merge_layer(merged, layer);
    }

    fmt::print("{}\n", answer_part1);
    print_image(layers);

    return 0;
}
