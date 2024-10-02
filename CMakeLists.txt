cmake_minimum_required(VERSION 3.25)
project(RBTree)

# C++-Standard 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
add_executable(RBTreeMain src/main.cpp)

# Create executable
add_executable(RBTreeTest src/test.cpp)

target_include_directories(RBTreeMain PRIVATE src)
target_include_directories(RBTreeTest PRIVATE src)

# Activate testing
enable_testing()

# Add test
add_test(NAME RBTreeTest COMMAND RBTreeTest)
