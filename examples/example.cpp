#include <iostream>
#include <Topname.h>

using namespace Topname;

enum class Planet {
    MERCURY, VENUS, EARTH, MARS, JUPITER, SATURN, URANUS, NEPTUNE
};

int main() {
    // Test 1: Basic initialization and conversion
    constexpr auto planet_names = EnumString(
        Planet::MERCURY, "Mercury",
        Planet::VENUS,   "Venus",
        Planet::EARTH,   "Earth",
        Planet::MARS,    "Mars",
        Planet::JUPITER, "Jupiter",
        Planet::SATURN,  "Saturn",
        Planet::URANUS,  "Uranus",
        Planet::NEPTUNE, "Neptune"
    );

    std::cout << "Test 1: Basic conversion" << std::endl;
    std::cout << planet_names.to_string(Planet::EARTH) << std::endl;
    std::cout << enum_to_underlying(planet_names.to_enum("Mars")) << std::endl;

    // Test 2: Error handling
    std::cout << "\nTest 2: Error handling" << std::endl;
    try {
        const auto name = planet_names.to_string(static_cast<Planet>(100));
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    try {
        const auto name = planet_names.to_enum("Pluto");
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    // Multiple mappings
    constexpr auto planet_types = EnumString(
        Planet::MERCURY, "Terrestrial",
        Planet::VENUS,   "Terrestrial",
        Planet::EARTH,   "Terrestrial",
        Planet::MARS,    "Terrestrial",
        Planet::JUPITER, "Gas Giant",
        Planet::SATURN,  "Gas Giant",
        Planet::URANUS,  "Ice Giant",
        Planet::NEPTUNE, "Ice Giant"
    );

    // Test 3: Case-insensitive comparison (if implemented)
    std::cout << "\nTest 3: Case-insensitive comparison" << std::endl;
    try {
        auto planet = planet_names.to_enum_insensitive("earth");
        std::cout << "Found planet: " << planet_names.to_string(planet) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Case-insensitive comparison not implemented" << std::endl;
    }

    // Test 4: Chaining operations
    std::cout << "\nTest 4: Chaining operations" << std::endl;
    std::cout << planet_types.to_string(planet_names.to_enum("Jupiter")) << std::endl;

    // Test 5: Using with standard algorithms
    std::cout << "\nTest 5: Using with standard algorithms" << std::endl;
    std::vector<Planet> all_planets;
    planet_names.for_each_enum([&all_planets](Planet p) { all_planets.push_back(p); });
    
    auto gas_giant = std::find_if(all_planets.begin(), all_planets.end(),
        [&planet_types](Planet p) { return planet_types.to_string(p) == "Gas Giant"; });
    
    if (gas_giant != all_planets.end()) {
        std::cout << "First gas giant: " << planet_names.to_string(*gas_giant) << std::endl;
    }

    // Test 6: Compile-time operations (if supported)
    std::cout << "\nTest 6: Compile-time operations" << std::endl;
    constexpr auto earth_name = planet_names.to_string(Planet::EARTH);
    std::cout << "Earth's name (compile-time): " << earth_name << std::endl;

    // Test 7: Performance test (basic)
    std::cout << "\nTest 7: Performance test" << std::endl;
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        volatile auto p = planet_names.to_enum("Earth");
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to perform " << iterations << " lookups: " << diff.count() << " s" << std::endl;

    return 0;
}
