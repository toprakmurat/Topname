#ifndef TOPNAME_H
#define TOPNAME_H

#include <algorithm> // std::ranges::find_if, std::ranges::for_each
#include <array>
#include <functional> // std::invoke
#include <stdexcept>
#include <string_view>
#include <type_traits>

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
    
    [[nodiscard]] constexpr E to_enum(std::string_view value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return pair.string_val == value; });
        
        if (it == mappings.end())
            throw std::out_of_range("String value not found in the mapping");
        
        return it->enum_val;
    }
    
    [[nodiscard]] constexpr std::string_view to_string(E value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return pair.enum_val == value; });
        
        if (it == mappings.end())
            throw std::out_of_range("Enum value not found in the mapping");
        
        return it->string_val;
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
    
private:
    struct EnumStringPair {
        E enum_val;
        std::string_view string_val;
    };
    
    std::array<EnumStringPair, N> mappings;
    
}; // class EnumString

}; // namespace Topname

#endif // TOPNAME_H
