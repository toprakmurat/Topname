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

// Hash function for better lookup performance
constexpr uint32_t hash(std::string_view str) {
    uint32_t hash = 5381;
    for (char ch : str) {
        hash = ((hash << 5) + hash) + ch; // hash * 33 + ch, but faster
    }
    return hash;
}

// Support case insensitive string comparison
constexpr bool case_insensitive_equal(std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](char c1, char c2) {
        return std::tolower(static_cast<unsigned char>(c1)) ==
               std::tolower(static_cast<unsigned char>(c2));
    });
}

template<EnumType E, std::size_t N>
class EnumString {
public:
    template<typename... Args>
    constexpr EnumString(Args&&... args)
    : mappings{{std::forward<Args>(args)...}}
    {
        build_hash_table();
    }

    // Average time complexity of a lookup: O(1)
    [[nodiscard]] constexpr E to_enum(std::string_view value) const {
        uint32_t h = hash(value) % HASH_TABLE_SIZE;
        while (hash_table[h].first != 0) {
            if (hash_table[h].first == hash(value)) {
                return hash_table[h].second;
            }
            h = (h + 1) % HASH_TABLE_SIZE;
        }
        throw std::out_of_range("String value not found in the mapping");
    }

    // Case insensitive to_enum function but average time complexity is O(n)
    [[nodiscard]] constexpr E to_enum_insensitive(std::string_view value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return case_insensitive_equal(pair.string_val, value);
        });

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

    [[nodiscard]] constexpr std::array<E, N> get_enum_all() const {
        std::array<E, N> res;
        for (std::size_t i = 0; i < N; i++) {
            res[i] = mappings[i].enum_val;
        }
        return res;
    }

    [[nodiscard]] constexpr std::array<std::string_view, N> get_string_all() const {
        std::array<E, N> res;
        for (std::size_t i = 0; i < N; i++) {
            res[i] = mappings[i].string_val;
        }
        return res;
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

    static constexpr std::size_t HASH_TABLE_SIZE = N * 2;
    std::array<std::pair<uint32_t, E>, HASH_TABLE_SIZE> hash_table{};

    constexpr void build_hash_table() {
        for (const auto& pair : mappings) {
            uint32_t h = hash(pair.string_val) % HASH_TABLE_SIZE;
            while (hash_table[h].first != 0) {
                h = (h + 1) % HASH_TABLE_SIZE;
            }
            hash_table[h] = {hash(pair.string_val), pair.enum_val};
        }
    }
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
