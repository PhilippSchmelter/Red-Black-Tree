# C++ Red-Black Tree Project

This project demonstrates a templated Red-Black Tree implementation in modern C++23. The tree supports common operations such as insertion, deletion, search, and various traversals.
## Features

- **Template-Based Design:** Works with various data types.
- **Full Red-Black Tree Functionality:** Supports insertion, deletion (with rebalancing), and search.
- **Traversals:** In-order, pre-order, and post-order traversal functions.
- **Validation:** Check if the tree satisfies all Red-Black Tree properties.
- **Modern C++23:** Utilizes modern C++ features for clean and efficient code.

## Project Structure

- **`RedBlackTree.h`**: Contains the complete Red-Black Tree implementation.
- **`main.cpp`**: Demonstrates how to use the Red-Black Tree with example operations.
- **`CMakeLists.txt`**: Build configuration for CMake.
- **`.gitignore`**: Git ignore file for a CLion C++23 project.
- **`README.md`**: This file.

## Requirements

- **Compiler:** A C++23 compliant compiler (e.g., GCC, Clang, or MSVC).
- **CMake:** Version 3.15 or later.
- **IDE:** CLion or any preferred C++ development environment.

## Building and Running

1. **Clone the Repository:**
   ```bash
   git clone <repository-url>
   cd <repository-folder>
   ```
2. **Build the Project:**
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
3. **Run the Executable:**
   ```bash
   ./<executable-name>
   ```

## Usage
The example in main.cpp demonstrates:

-Inserting key-value pairs into the Red-Black Tree.
-Performing in-order, pre-order, and post-order traversals.
-Searching for specific keys.
-Deleting nodes and validating the tree properties.

Feel free to modify main.cpp to experiment with additional operations or to integrate the Red-Black Tree into your projects.

##License
This project is open source. MIT License
