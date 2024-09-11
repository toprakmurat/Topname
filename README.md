# Topname

A modern C++ library for enhanced enum functionality, providing seamless conversion between enum values and their string representations. It includes a variety of useful utilities such as hashing functions, case-insensitive string comparison, and powerful, iterator-based access to enum-string pairs. The library is highly customizable and aims to offer optimal performance for common enum-related tasks.

## Features

- Topname allows you to map enum values to their corresponding string representations efficiently. It provides both case-sensitive and case-insensitive lookups for string-to-enum conversions.
- Topname employs a hashing mechanism based on the djb2 algorithm to efficiently map strings to enum values. The hash table optimizes lookup times to O(1) on average.
- Topname provides a random access iterator for traversing the enum-string mappings. You can iterate over pairs of enum values and strings or even over each of them separately.
- `EnumType Concept`: This concept ensures that only types marked as enums can be used with Topname's functions.
  
  `Compile-Time Enum Mapping Initialization`: Mappings are defined at compile-time for efficiency and simplicity.

- Header-only implementation for easy integration

This library aims to simplify working with enums in C++, reducing boilerplate code and enhancing type safety. It's designed to be efficient, flexible, and easy to use in C++ projects.

Still under development, any contribution is appreciated.
