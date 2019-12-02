#pragma once

#include <fmt/format.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <set>
#include <locale>
#include <string_view>
#include <charconv>

#if !defined(DEBUG)
#define DEBUG 0
#endif

namespace fmt {
    template <typename T>
    struct formatter<std::vector<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::vector<T> &v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", v.at(0));
                for (auto i = 1; i < v.size(); i++)
                    format_to(ctx.out(), ", {}", v.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };
}

namespace aoc {
    inline std::string_view trim(std::string_view sv) {
        while (std::isspace(sv.front())) sv.remove_prefix(1);
        while (std::isspace(sv.back())) sv.remove_suffix(1);
        return sv;
    }
}
