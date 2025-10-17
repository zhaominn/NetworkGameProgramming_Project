#pragma once
#include <tuple>
#include <functional>

// 템플릿 파라미터를 명확히 지정
struct TupleHash {
    template <typename T>
    static void hashCombine(std::size_t& seed, const T& value) {
        seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename... Types>
    std::size_t operator()(const std::tuple<Types...>& tuple) const {
        std::size_t seed = 0;
        std::apply([&seed](const auto&... args) {
            (hashCombine(seed, args), ...);
        }, tuple);
        return seed;
    }
};