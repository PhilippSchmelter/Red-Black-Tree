# rbtree — header-only C++20 Red-Black Tree

[![CI](https://github.com/PhilippSchmelter/Red-Black-Tree/actions/workflows/ci.yml/badge.svg)](https://github.com/PhilippSchmelter/Red-Black-Tree/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)

A small, single-header, templated self-balancing BST. Provides a `std::map`-shaped
API (`insert`, `erase`, `find`, `at`, `contains`, iterators, custom `Compare`) plus
explicit pre/in/post-order visitor traversals and an O(n) `validate()` that checks
every Red-Black invariant.

## Highlights

- Header-only: drop `include/rbtree/red_black_tree.hpp` into your project, or consume via CMake.
- CLRS-style **NIL sentinel**: simpler, faster fix-up routines than the null-pointer variant.
- Template visitors — no `std::function` indirection in the hot path.
- Move-aware, `noexcept`-correct, `[[nodiscard]]`-marked.
- Exception-safe copy (clones are torn down on partial failure).
- Bidirectional iterators with full STL semantics, including correct `--end()`.
- Works with non-default-constructible `Key`/`Value` types.
- Tested with **doctest** across GCC, Clang and MSVC under ASan/UBSan.

## Quick start

```cpp
#include <rbtree/red_black_tree.hpp>
#include <iostream>
#include <string>

int main() {
    rbtree::RedBlackTree<int, std::string> tree;
    tree.insert(10, "ten");
    tree.insert(20, "twenty");
    tree.insert(5,  "five");

    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << it.key() << " -> " << it.value() << '\n';
    }

    if (tree.contains(10)) tree.erase(10);
    std::cout << "valid: " << std::boolalpha << tree.validate() << '\n';
}
```

## API at a glance

| Category    | Members                                                              |
|-------------|----------------------------------------------------------------------|
| Capacity    | `empty()`, `size()`                                                  |
| Lookup      | `find()`, `contains()`, `at()`                                       |
| Modifiers   | `insert(k, v) -> {iterator, bool}`, `erase(k) -> bool`, `erase(it)`, `clear()`, `swap()` |
| Iteration   | `begin()`, `end()` (bidirectional)                                   |
| Traversal   | `in_order(visit)`, `pre_order(visit)`, `post_order(visit)`           |
| Validation  | `validate()` — verifies all five Red-Black properties + BST order    |

`Compare` defaults to `std::less<Key>`; pass `std::greater<Key>` for descending order
or any strict-weak-ordering callable. `Value` defaults to `Key` for set-like usage.

## Building

Requires CMake ≥ 3.16 and a C++20 compiler.

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

CMake options:

| Option                         | Default | Description                          |
|--------------------------------|---------|--------------------------------------|
| `RBTREE_BUILD_TESTS`           | ON\*    | Build the doctest test suite         |
| `RBTREE_BUILD_EXAMPLES`        | ON\*    | Build `examples/demo.cpp`            |
| `RBTREE_INSTALL`               | ON\*    | Generate install / package config    |
| `RBTREE_WARNINGS_AS_ERRORS`    | OFF     | `-Werror` / `/WX` on test+example targets |

\* default `ON` only when this is the top-level project; `OFF` when consumed via `add_subdirectory`.

## Consuming via CMake

```cmake
add_subdirectory(third_party/rbtree)
target_link_libraries(my_app PRIVATE rbtree::rbtree)
```

Or after `cmake --install build`:

```cmake
find_package(rbtree CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE rbtree::rbtree)
```

## Project layout

```
include/rbtree/red_black_tree.hpp   — the library
examples/demo.cpp                   — usage walkthrough
tests/test_red_black_tree.cpp       — doctest test suite
tests/doctest/doctest.h             — vendored doctest single header
CMakeLists.txt                      — modern CMake target + install
.github/workflows/ci.yml            — Linux/macOS/Windows + GCC/Clang/MSVC + sanitizers
```

## License

MIT — see [LICENSE](LICENSE).
