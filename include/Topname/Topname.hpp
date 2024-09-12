#ifndef TOPNAME_H
#define TOPNAME_H

#include <algorithm> // std::ranges::find_if, std::ranges::for_each
#include <array>
#include <functional> // std::invoke
#include <iterator> // for std::random_access_iterator_tag
#include <ostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Topname {
/**
 * @brief Exception class for EnumString-related errors.
 *
 * This class derives from std::runtime_error and provides additional
 * information about the specific type of error that occurred in the
 * EnumString operations.
 */
class EnumStringException : public std::runtime_error {
public:
    enum class ErrorCode {
        InvalidEnumValue,
        InvalidStringValue,
        OutOfRange,
        // Add more error codes as needed
    };
    
    EnumStringException(ErrorCode error, const std::string& message)
    : std::runtime_error(message), m_error_code(error) {}
    
    ErrorCode error_code() const { return m_error_code; }

private:
    ErrorCode m_error_code;
};

/**
 * @brief Concept to check if a type is an enum.
 * 
 * @tparam E The type to be checked.
 */
template<typename E>
concept EnumType = std::is_enum_v<E>;

/**
 * @brief Converts an enum value to its underlying type.
 * 
 * @tparam E Enum type.
 * @param enum_value The enum value to be converted.
 * @return The underlying value corresponding to the enum.
 */
template<EnumType E>
constexpr auto enum_to_underlying(E enum_value) noexcept {
    return static_cast<std::underlying_type_t<E>>(enum_value);
}

/**
 * @brief Computes a hash value for a string using the djb2 algorithm.
 * 
 * @param str The string to hash.
 * @return The computed hash value.
 */
constexpr uint32_t hash(std::string_view str) {
    uint32_t hash = 5381;
    for (char ch : str) {
        hash = ((hash << 5) + hash) + ch; // hash * 33 + ch, but faster
    }
    return hash;
}

/**
 * @brief Compares two strings for equality in a case-insensitive manner.
 * 
 * @param a The first string to compare.
 * @param b The second string to compare.
 * @return True if the strings are equal (case-insensitive), false otherwise.
 */
constexpr bool case_insensitive_equal(std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](char c1, char c2) {
        return std::tolower(static_cast<unsigned char>(c1)) ==
               std::tolower(static_cast<unsigned char>(c2));
    });
}

/**
 * @brief A class that maps enum values to corresponding strings and vice versa.
 * 
 * Provides efficient lookups and supports both case-sensitive and case-insensitive comparisons.
 * 
 * @tparam E Enum type.
 * @tparam N The number of mappings.
 */
template<EnumType E, std::size_t N>
class EnumString {
private:
    /**
     * @brief A struct representing an enum-string pair.
     */
    struct EnumStringPair {
        E enum_val;
        std::string_view string_val;
    };

    std::array<EnumStringPair, N> mappings; /**< The array of enum-string pairs. */

    static constexpr std::size_t HASH_TABLE_SIZE = N * 2;
    std::array<std::pair<uint32_t, E>, HASH_TABLE_SIZE> hash_table{};

    /**
     * @brief Builds the hash table for quicker lookups.
     */
    constexpr void build_hash_table() {
        for (const auto& pair : mappings) {
            uint32_t h = hash(pair.string_val) % HASH_TABLE_SIZE;
            while (hash_table[h].first != 0) {
                h = (h + 1) % HASH_TABLE_SIZE;
            }
            hash_table[h] = {hash(pair.string_val), pair.enum_val};
        }
    }

public:
    /**
     * @brief Constructs an EnumString with a list of enum-string pairs.
     * 
     * @tparam Args Variadic template arguments for the mappings.
     * @param args The mappings in pairs of enum values and their corresponding strings.
     */
    template<typename... Args>
    constexpr EnumString(Args&&... args)
    : mappings{{std::forward<Args>(args)...}}
    {
        build_hash_table();
    }

    /**
     * @brief Converts a string to its corresponding enum value.
     * 
     * Average time complexity: O(1).
     * 
     * @param value The string to convert.
     * @return The corresponding enum value.
     * @throw InvalidStringValue If the string does not match any enum value.
     */
    [[nodiscard]] constexpr E to_enum(std::string_view value) const {
        uint32_t h = hash(value) % HASH_TABLE_SIZE;
        while (hash_table[h].first != 0) {
            if (hash_table[h].first == hash(value)) {
                return hash_table[h].second;
            }
            h = (h + 1) % HASH_TABLE_SIZE;
        }
        auto err = EnumStringException::ErrorCode::InvalidStringValue;
        throw EnumStringException(err, "String value not found in the mapping");
    }

    /**
     * @brief Converts a string to its corresponding enum value (case-insensitive).
     * 
     * Average time complexity: O(n).
     * 
     * @param value The string to convert.
     * @return The corresponding enum value.
     * @throw InvalidStringValue If the string does not match any enum value.
     */
    [[nodiscard]] constexpr E to_enum_insensitive(std::string_view value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return case_insensitive_equal(pair.string_val, value);
        });

        if (it == mappings.end()) {
            auto err = EnumStringException::ErrorCode::InvalidStringValue;
            throw EnumStringException(err, "String value not found in the mapping");
        }

        return it->enum_val;
    }

    /**
     * @brief Converts an enum value to its corresponding string.
     * 
     * @param enum_value The enum value to convert.
     * @return The corresponding string.
     * @throw InvalidEnumValue If the enum value does not match any string.
     */
    [[nodiscard]] constexpr std::string_view to_string(E value) const {
        auto it = std::ranges::find_if(mappings, [value](const auto& pair) {
            return pair.enum_val == value; });

        if (it == mappings.end()) {
            auto err = EnumStringException::ErrorCode::InvalidEnumValue;
            throw EnumStringException(err, "Enum value not found in the mapping");
        }
        return it->string_val;
    }

    /**
     * @brief Retrieves all enum values from the mapping.
     * 
     * @return A vector containing all enum values.
     */
    [[nodiscard]] constexpr std::array<E, N> get_enum_all() const {
        std::array<E, N> res;
        for (std::size_t i = 0; i < N; i++) {
            res[i] = mappings[i].enum_val;
        }
        return res;
    }

    /**
     * @brief Retrieves all string values from the mapping.
     * 
     * @return A vector containing all string values.
     */
    [[nodiscard]] constexpr std::array<std::string_view, N> get_string_all() const {
        std::array<E, N> res;
        for (std::size_t i = 0; i < N; i++) {
            res[i] = mappings[i].string_val;
        }
        return res;
    }

    /**
     * @brief Checks if a given enum value exists in the mapping.
     * 
     * @param enum_value The enum value to check.
     * @return True if the enum value exists in the mapping, false otherwise.
     */
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

    /**
     * @brief Checks if a given string value exists in the mapping.
     * 
     * @param string_value The string value to check.
     * @return True if the string value exists in the mapping, false otherwise.
     */
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

    /**
     * @brief Applies a function to each enum value in the mapping.
     * 
     * @tparam Func The type of the function to apply.
     * @param func The function to apply.
     */
    template<typename Func>
    constexpr void for_each_enum(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.enum_val);
        });
    }

    /**
     * @brief Applies a function to each string in the mapping.
     * 
     * @tparam Func The type of the function to apply.
     * @param func The function to apply.
     */
    template<typename Func>
    constexpr void for_each_string(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.string_val);
        });
    }

    /**
     * @brief Applies a function to each enum-string pair in the mapping.
     * 
     * @tparam Func The type of the function to apply.
     * @param func The function to apply.
     */
    template<typename Func>
    constexpr void for_each_pair(Func&& func) const {
        std::ranges::for_each(mappings, [&func](const auto& pair) {
            std::invoke(std::forward<Func>(func), pair.enum_val, pair.string_val);
        });
    }

    /**
     * @brief Overloads the '<<' operator for EnumString.
     * 
     * Outputs the string representation of all enum-string pairs.
     * 
     * @tparam F The enum type.
     * @tparam M The number of mappings.
     * @param os The output stream.
     * @param enum_str The EnumString instance.
     * @return The output stream.
     */
    template<EnumType F, std::size_t M>
    friend std::ostream& operator<<(std::ostream& os, const EnumString<F, M>& enum_str);

    /**
     * @brief Overloads the '<<' operator for enum values.
     * 
     * Outputs the string corresponding to the given enum value.
     * 
     * @tparam F The enum type.
     * @tparam M The number of mappings.
     * @param os The output stream.
     * @param enum_val The enum value.
     * @return The output stream.
     */
    template<EnumType F, std::size_t M>
    friend std::ostream& operator<<(std::ostream& os, F enum_val);

    /**
     * @class Iterator
     * @brief A random access iterator for EnumStringPair objects.
     * 
     * Provides basic functionality for iterating through EnumStringPair objects
     * using random access. Supports standard iterator operations like increment,
     * decrement, pointer dereferencing, and arithmetic.
     */
    class Iterator {
    private:
        const EnumStringPair* m_ptr;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = const EnumStringPair;
        using difference_type = ptrdiff_t;
        using pointer = const EnumStringPair*;
        using reference = const EnumStringPair&;
        
        Iterator(pointer ptr) : m_ptr(ptr) {}
        
        reference operator*() const { return *m_ptr; }
        pointer operator->() const { return m_ptr; }
        
        Iterator& operator++() { ++m_ptr; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        
        Iterator& operator--() { --m_ptr; return *this; }
        Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }
        
        Iterator& operator+=(difference_type n) { m_ptr += n; return *this; }
        Iterator operator+(difference_type n) const { return Iterator(m_ptr + n); }
        
        Iterator& operator-=(difference_type n) { m_ptr -= n; return *this; }
        Iterator operator-(difference_type n) const { return Iterator(m_ptr - n); }
        
        difference_type operator+(const Iterator& other) const { return m_ptr + other.m_ptr; }
        difference_type operator-(const Iterator& other) const { return m_ptr - other.m_ptr; }
        
        reference operator[](difference_type n) const { return m_ptr[n]; }
        
        bool operator==(const Iterator& other) const { return m_ptr == other.m_ptr; }
        bool operator!=(const Iterator& other) const { return m_ptr != other.m_ptr; }
        bool operator<=(const Iterator& other) const { return m_ptr <= other.m_ptr; }
        bool operator>=(const Iterator& other) const { return m_ptr >= other.m_ptr; }
        bool operator<(const Iterator& other) const { return m_ptr < other.m_ptr; }
        bool operator>(const Iterator& other) const { return m_ptr > other.m_ptr; }
    };
    
    /**
     * @brief Returns an iterator to the beginning of the collection.
     * 
     * @return An iterator pointing to the first element in the collection.
     */
    [[nodiscard]] Iterator begin() const { return Iterator(mappings.data()); }
    
    /**
     * @brief Returns an iterator to the end of the collection.
     * 
     * @return An iterator pointing to one past the last element in the collection.
     */
    [[nodiscard]] Iterator end() const { return Iterator(mappings.data() + N); }
    
    /**
     * @brief Returns a reverse iterator to the last element of the collection.
     * 
     * @return A reverse iterator pointing to the last element.
     */
    [[nodiscard]] Iterator rbegin() const { return Iterator(mappings.data() + N - 1); }

    /**
     * @brief Returns a reverse iterator to one before the first element of the collection.
     * 
     * @return A reverse iterator pointing to one before the first element.
     */
    [[nodiscard]] Iterator rend() const { return Iterator(mappings.data() - 1); }

}; // class EnumString

/**
 * @brief Outputs all the string representations in an EnumString instance.
 * 
 * @tparam E The enum type.
 * @tparam N The number of mappings.
 * @param os The output stream.
 * @param enum_str The EnumString instance.
 * @return The output stream.
 */
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

/**
 * @brief Deduction guide for the compile to construct an EnumString struct.
 * 
 * @tparam E The enum type.
 * @tparam Args Variadic template arguments.
 */
template<EnumType E, typename... Args>
EnumString(E, std::string_view, Args...) -> EnumString<E, sizeof...(Args)/2 + 1>;

}; // namespace Topname

#endif // TOPNAME_H
