#pragma once

#include <fmt/format.h>
#include <iostream>
#include <cstdint>
#include <vector>
#include <set>
#include <locale>

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
    inline void set_input_delimiters(const std::vector<char>& delims) {
        using cctype = std::ctype<char>;
        const auto original_table = cctype::classic_table();
        std::vector<cctype::mask> our_table(original_table, original_table + cctype::table_size);
        for (auto& m : our_table)
            m &= ~std::ctype_base::space;
        for (auto c : delims)
            our_table[c] |= std::ctype_base::space;
        std::cin.imbue(std::locale(std::cin.getloc(), new cctype(our_table.data())));
    }

    inline void reset_input_delimiters() {
        std::cin.imbue(std::locale());
    }
}
