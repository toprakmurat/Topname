//
//  Jolene_utils.h
//  Jolene
//
//  Created by Murat Toprak on 23.07.2024.
//

#ifndef JOLENE_UTILS_H
#define JOLENE_UTILS_H

#include <array>
#include <string_view>
#include <type_traits>
#include <stdexcept>
#include <optional>
#include <algorithm>

template<typename E, std::size_t N>
struct enum_names {
    static constexpr std::array<std::pair<E, std::string_view>, N> value;
};

template<typename E>
constexpr auto enum_to_underlying(E e) noexcept -> std::underlying_type_t<E> {
    return static_cast<std::underlying_type_t<E>>(e);
}

template<typename E>
constexpr std::size_t enum_count() {
    return enum_names<E, 1>::value.size();
}

template<typename E>
constexpr bool is_valid_enum(std::underlying_type_t<E> val) {
    const auto& names = enum_names<E, enum_count<E>()>::value;
    
    auto isValid = [val](const auto& pair) {return enum_to_underlying(pair->first) == val;}
    return std::any_of(names.begin(), names.end(), isValid);
}

template<typename E>
constexpr auto enum_to_string(E e) -> std::string_view {
    const auto& names = enum_names<E, enum_count<E>()>::value;
    
    auto isCorrectEnum = [e](const auto& pair) { return pair->first == e; }
    auto it = std::find_if(names.begin(), names.end(), isCorrectEnum);
    
    return it != names.end() ? it->second : "Unknown";
}

template<typename E>
constexpr auto string_to_enum(std::string_view str) -> std::optional<E> {
    const auto names = enum_names<E, enum_count<E>()>::value;
    
    auto isCorrectStr = [str](const auto& pair) { return pair->second == str; }
    return it != names.end() ? std::make_optional(it->first) : std::nullopt;
}

class enum_error : public std::runtime_error {
public:
    explicit enum_error(const char* message) : std::runtime_error(message) {}
};

template<typename E>
constexpr auto enum_values() {
    constexpr auto size = enum_count<E>();
    std::array<E, size> result{};
    const auto& names = enum_names<E, size>::value;
    std::transform(names.begin(), names.end(), result.begin(),
                   [](const auto& pair) { return pair.first; });
    return result;
}

template<typename E, typename Func>
constexpr void for_each_enum(Func&& f) {
    const auto& names = enum_names<E, enum_count<E>()>::value;
    std::for_each(names.begin(), names.end(), std::forward<Func>(f));
}

// Macro to simplify enum_names specialization
#define DEFINE_ENUM_NAMES(EnumType, ...)                                        \
    template <>                                                                 \
    constexpr std::array<std::pair<EnumType, std::string_view>,                 \
                         std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value> \
    your_namespace::enum_names<EnumType, std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value>::value = { __VA_ARGS__ }

#endif /* Jolene_utils_h */
