
# Red-Black Tree Implementation in C++

This repository contains an implementation of a Red-Black Tree in C++. A Red-Black Tree is a self-balancing binary search tree that maintains O(log n) time complexity for insertion, deletion, and search operations. This implementation is designed to be efficient, easy to use, and well-tested.

## Features

- **Insertion**: Adds elements while keeping the tree balanced.
- **Deletion**: Removes elements and ensures the tree remains balanced.
- **Search**: Efficiently finds elements in O(log n) time.
- **Traversal**: Supports in-order tree traversals.
- **Unit Tests**: Comprehensive unit tests to validate all tree operations.

## Getting Started

### Prerequisites

- A modern C++ compiler that supports C++20 or higher (e.g., GCC, Clang, MSVC).
- [CMake](https://cmake.org/) for building the project.

### Building the Project

1. Clone the repository:

    ```bash
    git clone https://github.com/PhilippSchmelter/red-black-tree.git
    cd red-black-tree
    ```

2. Create a build directory and run CMake:

    ```bash
    cmake -S . -B build
    cmake --build build
    ```

### Running the Tests

To run the tests, use `ctest` from the build directory:

```bash
ctest --test-dir build --output-on-failure
```

If all tests pass, you'll see the message "All tests passed!"

### Running the Main Program

After building the project, you can run the main program:

```bash
./build/RBTreeMain
```

## Contributing

Contributions are welcome! Please feel free to open an issue or submit a pull request for any bugs, improvements, or new features.

## License

This project is licensed under the MIT License. See the [LICENSE.md](LICENSE.md) file for details.
