# KEEL Optional

`keel::Optional` is a lightweight, header-only, C++17+ implementation of an optional type, inspired by `std::optional`. It allows you to store an object that may or may not contain a value and provides safe access with minimal overhead.

## Features

- Fully C++17 and later compatible.
- No dynamic allocations.
- Provides move and copy semantics.
- Supports in-place construction with `Emplace`.
- Customizable assert for runtime checks.
- constexpr support for compile-time evaluation (C++20) or inline fallback (C++17).

## Header

All functionality is contained in a single header file:

    #include "Optional.h"

No additional dependencies are required beyond standard C++ headers.

## Usage

### Basic Example
```cpp
    #include "Optional.h"
    #include <iostream>

    int main() {
        keel::Optional<int> a;             // Empty optional
        keel::Optional<int> b(42);         // Optional with value

        if (!a) {
            std::cout << "a has no value\n";
        }

        if (b.HasValue()) {
            std::cout << "b contains: " << *b << "\n";
        }

        a.Emplace(10);                     // In-place construction
        std::cout << "a now contains: " << a.GetValue() << "\n";

        return 0;
    }
```
### Comparison Operators
```cpp
    keel::Optional<int> x(5);
    keel::Optional<int> y(5);
    keel::Optional<int> z;

    if (x == y) { /* true */ }
    if (x != z) { /* true */ }
```
### Utility Traits

You can check if a type is `keel::Optional` using:
```cpp
    static_assert(keel::IsOptional<decltype(x)>::Value, "x is an Optional");
```
## API Overview

- **Constructors**: default, copy, move, from value.
- **Assignment Operators**: copy, move, assign from value.
- **Emplace**: construct a value in-place.
- **Reset**: destroy the contained value.
- **GetValue** / `operator*` / `operator->`: access the value (with assertion if empty).
- **Get**: access the value (with default value).
- **HasValue** / `operator bool`: check if the optional contains a value.
- **Comparison operators**: `==` and `!=`.

## Macros

- `KEEL_CONSTEXPR`: `constexpr` in C++20, inline fallback in C++17.
- `KEEL_ASSERT(expr, message)`: defaults to `assert`, can be overridden.

## Notes

- This implementation avoids `std::optional` to maintain independence from the standard library's optional implementation.
- Suitable for embedded and performance-sensitive environments since it does not allocate memory dynamically.

## License

This code is provided as-is under the MIT License. Feel free to use, modify, and distribute.
