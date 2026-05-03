cmake_minimum_required(VERSION 3.16)
project(rbtree
    VERSION   3.0.0
    LANGUAGES CXX
    DESCRIPTION "Header-only templated Red-Black Tree"
    HOMEPAGE_URL "https://github.com/PhilippSchmelter/Red-Black-Tree"
)

# Detect whether we are the top-level project so that downstream consumers via
# add_subdirectory() do not silently inherit our tests/examples/install logic.
set(RBTREE_IS_TOP_LEVEL OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(RBTREE_IS_TOP_LEVEL ON)
endif()

option(RBTREE_BUILD_TESTS    "Build unit tests"            ${RBTREE_IS_TOP_LEVEL})
option(RBTREE_BUILD_EXAMPLES "Build example executables"   ${RBTREE_IS_TOP_LEVEL})
option(RBTREE_INSTALL        "Generate install target"     ${RBTREE_IS_TOP_LEVEL})
option(RBTREE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# ---------------------------------------------------------------------------
# Library target — header-only INTERFACE
# ---------------------------------------------------------------------------
add_library(rbtree INTERFACE)
add_library(rbtree::rbtree ALIAS rbtree)

target_include_directories(rbtree INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(rbtree INTERFACE cxx_std_20)

# ---------------------------------------------------------------------------
# Strict warnings for first-party targets (tests + examples)
# ---------------------------------------------------------------------------
add_library(rbtree_warnings INTERFACE)
if(MSVC)
    target_compile_options(rbtree_warnings INTERFACE /W4 /permissive-)
    if(RBTREE_WARNINGS_AS_ERRORS)
        target_compile_options(rbtree_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(rbtree_warnings INTERFACE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnon-virtual-dtor
        -Wold-style-cast -Woverloaded-virtual -Wnull-dereference)
    if(RBTREE_WARNINGS_AS_ERRORS)
        target_compile_options(rbtree_warnings INTERFACE -Werror)
    endif()
endif()

# ---------------------------------------------------------------------------
# Examples
# ---------------------------------------------------------------------------
if(RBTREE_BUILD_EXAMPLES)
    add_executable(rbtree_demo examples/demo.cpp)
    target_link_libraries(rbtree_demo PRIVATE rbtree::rbtree rbtree_warnings)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(RBTREE_BUILD_TESTS)
    enable_testing()
    add_executable(rbtree_tests tests/test_red_black_tree.cpp)
    target_link_libraries(rbtree_tests PRIVATE rbtree::rbtree rbtree_warnings)
    target_include_directories(rbtree_tests PRIVATE tests)
    add_test(NAME rbtree_tests COMMAND rbtree_tests)
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
if(RBTREE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS rbtree EXPORT rbtreeTargets)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT rbtreeTargets
        FILE        rbtreeTargets.cmake
        NAMESPACE   rbtree::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rbtree)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/rbtreeConfigVersion.cmake"
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/rbtreeConfig.cmake"
        "include(\"\${CMAKE_CURRENT_LIST_DIR}/rbtreeTargets.cmake\")\n")

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/rbtreeConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/rbtreeConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rbtree)
endif()
