# Contributing

Thanks for your interest. This is a small library — contributions are welcome,
especially around correctness and portability.

## Development loop

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DRBTREE_WARNINGS_AS_ERRORS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The CI matrix runs the same flow across GCC, Clang and MSVC, plus an Address /
Undefined sanitizer pass on Linux. Please make sure tests pass locally and add
new ones for any behaviour change.

## Style

Source is formatted with `clang-format` (config in `.clang-format`). To apply:

```bash
clang-format -i include/rbtree/*.hpp tests/*.cpp examples/*.cpp
```

A `.clang-tidy` ruleset is included for static analysis; run with:

```bash
clang-tidy -p build include/rbtree/red_black_tree.hpp
```

## Adding tests

Tests live in `tests/test_red_black_tree.cpp` and use [doctest](https://github.com/doctest/doctest)
(vendored at `tests/doctest/doctest.h`). New cases go in their own `TEST_CASE`
and should call `validate()` after every meaningful mutation.

## Pull requests

- One topic per PR. Keep the diff focused.
- Explain the *why* in the description; the diff already shows the *what*.
- Make sure the CI matrix is green before requesting review.
