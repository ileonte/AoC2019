#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string_view>
#include <charconv>
#include <optional>
#include <functional>
#include <algorithm>
#include <numeric>
#include <array>
#include <deque>

#include <fmt/format.h>
#include <fmt/ostream.h>

#if !defined(DEBUG)
#define DEBUG 0
#endif

namespace fmt {
    template <typename T>
    struct formatter<std::vector<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::vector<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", v.at(0));
                for (auto i = 1; i < v.size(); i++)
                    format_to(ctx.out(), ", {}", v.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename T>
    struct formatter<std::deque<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::deque<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", v.at(0));
                for (auto i = 1; i < v.size(); i++)
                    format_to(ctx.out(), ", {}", v.at(i));
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename T>
    struct formatter<std::set<T>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::set<T>& v, FormatContext &ctx) {
            format_to(ctx.out(), "[");
            if (v.size() > 0) {
                format_to(ctx.out(), "{}", *std::cbegin(v));
                for (auto i = std::next(std::cbegin(v)); i != std::cend(v); i++)
                    format_to(ctx.out(), ", {}", *i);
            }
            return format_to(ctx.out(), "]");
        }
    };

    template <typename K, typename V>
    struct formatter<std::unordered_map<K, V>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::unordered_map<K, V>& m, FormatContext &ctx) {
            format_to(ctx.out(), "{{");
            for (const auto& [k, v] : m)
                format_to(ctx.out(), "\n   {} -> {}", k, v);
            return format_to(ctx.out(), "{}}}", m.size() ? "\n" : "");
        }
    };

    template <typename T1, typename T2>
    struct formatter<std::pair<T1, T2>> {
        template <typename ParseContext>
        constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

        template <typename FormatContext>
        auto format(const std::pair<T1, T2>& v, FormatContext &ctx) {
            return format_to(ctx.out(), "{{{}, {}}}", v.first, v.second);
        }
    };
}

namespace aoc {
    inline std::string_view trim(std::string_view sv) {
        while (std::isspace(sv.front())) sv.remove_prefix(1);
        while (std::isspace(sv.back())) sv.remove_suffix(1);
        return sv;
    }


    inline constexpr std::string_view
    substr(std::string_view the_string, size_t from = 0,
           size_t count = std::numeric_limits<size_t>::max())
    {
        if (from >= the_string.size()) return std::string_view();
        return std::string_view(the_string.data() + from, std::min(count, the_string.size() - from));
    }

    inline std::vector<std::string_view>
    str_split(std::string_view the_string, char sep,
              size_t max_splits = std::numeric_limits<size_t>::max())
    {
        std::vector<std::string_view> ret;
        size_t start = 0;
        size_t end = 0;
        while (start < the_string.size()) {
            end = the_string.find(sep, start);
            ret.push_back(substr(the_string, start, end - start));
            if (end >= the_string.size()) break;
            start = end + 1;
            if (ret.size() >= max_splits - 1) {
                if (start < the_string.size())
                    ret.push_back(substr(the_string, start));
                break;
            }
        }
        if (start == the_string.size() && !the_string.empty()) {
            ret.push_back("");
        }
        return ret;
    }

    template <typename T>
    constexpr inline T cpow(T base, T pow) {
        T ret{1};
        for (T p = 1; p <= pow; p++)
            ret *= base;
        return ret;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T gcd(T v1, T v2) {
        T n1 = std::abs(std::max(v1, v2));
        T n2 = std::abs(std::min(v1, v2));
        if (!n2) return 1;
        while (n2) {
            T t = n1;
            n1 = n2;
            n2 = t % n2;
        }
        return n1;
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T lcm(T v1, T v2) {
        return v1 * v2 / gcd(v1, v2);
    }

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    constexpr inline T bitmask(size_t size, size_t final_shift = 0) {
        T r{1};
        for (size_t i = 1; i < size; i++) r = (r << 1) | 1;
        return r << final_shift;
    }
}
