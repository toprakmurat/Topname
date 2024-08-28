#ifndef TOPNAME_H
#define TOPNAME_H

#include <algorithm> // std::ranges::find_if, std::ranges::for_each
#include <array>
#include <functional> // std::invoke
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Topname {

template<typename E>
concept EnumType = std::is_enum_v<E>;

template<EnumType E>
constexpr auto enum_to_underlying(E enum_value) noexcept {
    return static_cast<std::underlying_type_t<E>>(enum_value);
}

template<EnumType E, std::size_t N>
class EnumString {
public:
    template<typename... Args>
    constexpr EnumString(Args&&... args)
    : mappings{{std::forward<Args>(args)...}}
    {}

    [[nodiscard]] constexpr E to_enum_first(std::string_view value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return pair.string_val == value; });

        if (it == mappings.end())
            throw std::out_of_range("String value not found in the mapping");

        return it->enum_val;
    }

    [[nodiscard]] constexpr std::vector<E> to_enum_all(std::string_view value) const {
        std::vector<E> res;
        for_each_pair([&res, value](E enum_value, std::string_view str_value) {
            if (str_value == value) {
                res.push_back(enum_value);
            }
        });
        return res;
    }

    [[nodiscard]] constexpr std::string_view to_string(E value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return pair.enum_val == value; });

        if (it == mappings.end())
            throw std::out_of_range("Enum value not found in the mapping");

        return it->string_val;
    }

    constexpr bool contains(E target) const {
        bool found = false;
        for_each_enum([&found, target](E enum_val) {
            if (enum_val == target) {
                found = true;
                return;
            }
        });
        return found;
    }

    constexpr bool contains(std::string_view target) const {
        bool found = false;
        for_each_string([&found, target](std::string_view str_val) {
            if (str_val == target) {
                found = true;
                return;
            }
        });
        return found;
    }

    template<typename Func>
    constexpr void for_each_enum(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.enum_val);
        });
    }

    template<typename Func>
    constexpr void for_each_string(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.string_val);
        });
    }

    template<typename Func>
    constexpr void for_each_pair(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.enum_val, pair.string_val);
        });
    }

    template<EnumType F, std::size_t M>
    friend std::ostream& operator<<(std::ostream& os, const EnumString<F, M>& enum_str);

    template<EnumType F, std::size_t M>
    friend std::ostream& operator<<(std::ostream& os, F enum_val);

private:
    struct EnumStringPair {
        E enum_val;
        std::string_view string_val;
    };

    std::array<EnumStringPair, N> mappings;
}; // class EnumString

template<EnumType E, std::size_t N>
std::ostream& operator<<(std::ostream& os, const EnumString<E, N>& enum_str) {
    os << "EnumString{";
    for (std::size_t i = 0; i < N; i++) {
        if (i > 0) os << ", ";
        os << enum_str.mappings[i].string_val;
    }
    os << "}";

    return os;
}

// Deduction guide for the compiler
template<EnumType E, typename... Args>
EnumString(E, std::string_view, Args...) -> EnumString<E, sizeof...(Args)/2 + 1>;

}; // namespace Topname

#endif // TOPNAME_H
