#include <iostream>

#include <Topname.h>

using namespace Topname;

enum class Color {
    RED,
    GREEN,
    BLUE
};

int main() {
    // Initialize EnumString class
    constexpr auto colors_hex = EnumString<Color, 3>(
        Color::RED,     "0xff0000",
        Color::GREEN,   "0x00ff00",
        Color::BLUE,    "0x0000ff"
    );
    
    // Using for_each_enum
    colors_hex.for_each_enum([](Color c) {
        std::cout << "Enum Value: " << enum_to_underlying(c) << std::endl;
    });
    
    // Using for_each_string
    colors_hex.for_each_string([](std::string_view str) {
        std::cout << "String value: " << str << std::endl;
    });
    
    // Using for_each_pair
    colors_hex.for_each_pair([](Color c, std::string_view str) {
        std::cout << "Enum: " << enum_to_underlying(c) << " String: " << str << std::endl;
    });
}