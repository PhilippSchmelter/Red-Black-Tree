# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.0.0] — 2026-05-03

### Added
- Header relocated to `include/rbtree/red_black_tree.hpp` and placed in namespace `rbtree`.
- CLRS-style NIL sentinel — fix-up routines no longer null-check every neighbour.
- Bidirectional `const_iterator` with correct `--end()` semantics.
- `Compare` template parameter (defaults to `std::less<Key>`).
- `size()`, `empty()`, `clear()`, `contains()`, `at()`, `erase(iterator)`, `swap()`.
- Move constructor and move-assignment operator.
- `std::pair<iterator, bool>` return from `insert` (no exception on duplicate).
- Doctest test suite under `tests/`, wired into CTest.
- CMake INTERFACE library target `rbtree::rbtree` with install + package-config.
- CI matrix across Ubuntu/macOS/Windows × GCC/Clang/MSVC, plus ASan/UBSan jobs.
- `.clang-format`, `.clang-tidy`, `.editorconfig`.

### Changed
- Traversal callbacks are template parameters; `std::function` indirection removed.
- `Color` is now `enum class Color : unsigned char`.
- `clone` is exception-safe — partial subtrees are torn down on failure.
- Iterative subtree destruction; deep trees no longer risk stack overflow.
- README rewritten with badges, API reference, build and consumption instructions.

### Removed
- The old top-level `RedBlackTree.h` and `main.cpp`. Their contents moved to
  `include/rbtree/red_black_tree.hpp` and `examples/demo.cpp` respectively.
- Public `Node*` exposure from query methods; use the iterator API instead.

## [2.0.0]

Initial public release of the templated Red-Black Tree (single header `RedBlackTree.h`).
