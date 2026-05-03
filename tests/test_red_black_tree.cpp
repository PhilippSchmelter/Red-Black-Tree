// SPDX-License-Identifier: MIT
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include "rbtree/red_black_tree.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <vector>

using rbtree::RedBlackTree;

namespace {

template <typename Tree>
std::vector<typename Tree::key_type> in_order_keys(const Tree& t) {
    std::vector<typename Tree::key_type> out;
    out.reserve(t.size());
    t.in_order([&](const auto& k, const auto&) { out.push_back(k); });
    return out;
}

} // namespace

TEST_CASE("default-constructed tree is empty and valid") {
    RedBlackTree<int, std::string> t;
    CHECK(t.empty());
    CHECK(t.size() == 0);
    CHECK(t.validate());
    CHECK(t.begin() == t.end());
    CHECK_FALSE(t.contains(42));
}

TEST_CASE("insert returns the right pair and rejects duplicates") {
    RedBlackTree<int, std::string> t;

    auto [it1, ok1] = t.insert(10, "ten");
    CHECK(ok1);
    CHECK(it1.key() == 10);
    CHECK(it1.value() == "ten");

    auto [it2, ok2] = t.insert(10, "TEN");
    CHECK_FALSE(ok2);
    CHECK(it2 == it1);
    CHECK(t.size() == 1);
    CHECK(t.at(10) == "ten"); // value is unchanged
}

TEST_CASE("validate detects properly-shaped trees") {
    RedBlackTree<int> t;
    for (int v : {10, 20, 30, 15, 25, 5, 1, 40, 35, 50, 60, 70, 80}) {
        t.insert(v, v);
        CHECK(t.validate());
    }
    CHECK(t.size() == 13);
}

TEST_CASE("in-order traversal yields sorted keys") {
    RedBlackTree<int> t;
    std::vector<int> input{42, 17, 9, 88, 33, 1, 56, 4, 77, 28};
    for (int v : input) t.insert(v, v);

    auto sorted = input;
    std::sort(sorted.begin(), sorted.end());
    CHECK(in_order_keys(t) == sorted);
}

TEST_CASE("iterator round-trip matches in_order traversal") {
    RedBlackTree<int> t;
    for (int v : {5, 3, 8, 1, 4, 7, 9}) t.insert(v, v);

    std::vector<int> via_iter;
    for (auto it = t.begin(); it != t.end(); ++it) via_iter.push_back(it.key());

    CHECK(via_iter == in_order_keys(t));

    // bidirectional walk back
    std::vector<int> reverse;
    auto it = t.end();
    while (it != t.begin()) {
        --it;
        reverse.push_back(it.key());
    }
    std::reverse(reverse.begin(), reverse.end());
    CHECK(reverse == via_iter);
}

TEST_CASE("erase removes nodes and preserves invariants") {
    RedBlackTree<int> t;
    for (int v : {10, 20, 30, 15, 25, 5, 1, 40, 35, 50}) t.insert(v, v);
    REQUIRE(t.validate());

    CHECK(t.erase(20));
    CHECK_FALSE(t.contains(20));
    CHECK(t.size() == 9);
    CHECK(t.validate());

    CHECK_FALSE(t.erase(20));        // already gone
    CHECK_FALSE(t.erase(9999));      // never present

    // erase the root repeatedly until the tree is empty
    while (!t.empty()) {
        auto k = t.begin().key();
        CHECK(t.erase(k));
        CHECK(t.validate());
    }
    CHECK(t.empty());
    CHECK(t.begin() == t.end());
}

TEST_CASE("at throws on missing key") {
    RedBlackTree<int, std::string> t;
    t.insert(1, "one");
    CHECK(t.at(1) == "one");
    CHECK_THROWS_AS((void)t.at(2), std::out_of_range);
}

TEST_CASE("clear empties the tree without leaks") {
    RedBlackTree<int, std::string> t;
    for (int i = 0; i < 100; ++i) t.insert(i, std::to_string(i));
    CHECK(t.size() == 100);
    t.clear();
    CHECK(t.empty());
    CHECK(t.validate());
    // still usable afterwards
    t.insert(1, "one");
    CHECK(t.size() == 1);
}

TEST_CASE("copy constructor performs a deep copy") {
    RedBlackTree<int, std::string> a;
    for (int i = 0; i < 50; ++i) a.insert(i, std::to_string(i));

    RedBlackTree<int, std::string> b(a);
    CHECK(b.size() == a.size());
    CHECK(b.validate());
    CHECK(in_order_keys(a) == in_order_keys(b));

    // mutating b must not touch a
    CHECK(b.erase(25));
    CHECK_FALSE(b.contains(25));
    CHECK(a.contains(25));
}

TEST_CASE("copy assignment is exception-safe and self-assignment is a no-op") {
    RedBlackTree<int> a;
    for (int v : {1, 2, 3, 4, 5}) a.insert(v, v);

    RedBlackTree<int> b;
    b = a;
    CHECK(in_order_keys(b) == in_order_keys(a));

    a = a; // self-assignment
    CHECK(in_order_keys(a) == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("move constructor and move assignment leave the source empty") {
    RedBlackTree<int> a;
    for (int v : {1, 2, 3, 4, 5}) a.insert(v, v);

    RedBlackTree<int> b(std::move(a));
    CHECK(b.size() == 5);
    CHECK(a.empty());
    CHECK(a.validate());

    RedBlackTree<int> c;
    c = std::move(b);
    CHECK(c.size() == 5);
    CHECK(b.empty());
}

TEST_CASE("custom Compare yields descending order") {
    RedBlackTree<int, int, std::greater<int>> t;
    for (int v : {3, 1, 4, 1, 5, 9, 2, 6}) t.insert(v, v);
    auto keys = in_order_keys(t);
    CHECK(std::is_sorted(keys.begin(), keys.end(), std::greater<int>{}));
}

TEST_CASE("randomised stress: insert and erase a large set, invariants hold") {
    constexpr int N = 4000;
    std::mt19937 rng(0xC0FFEE);

    std::vector<int> keys(N);
    std::iota(keys.begin(), keys.end(), 0);
    std::shuffle(keys.begin(), keys.end(), rng);

    RedBlackTree<int> t;
    for (int k : keys) {
        t.insert(k, k);
    }
    REQUIRE(t.size() == static_cast<std::size_t>(N));
    REQUIRE(t.validate());

    auto sorted = in_order_keys(t);
    CHECK(std::is_sorted(sorted.begin(), sorted.end()));

    std::shuffle(keys.begin(), keys.end(), rng);
    int removed = 0;
    for (int k : keys) {
        CHECK(t.erase(k));
        ++removed;
        if (removed % 250 == 0) REQUIRE(t.validate());
    }
    CHECK(t.empty());
    CHECK(t.validate());
}

TEST_CASE("erase via iterator returns the in-order successor") {
    RedBlackTree<int> t;
    for (int v : {1, 2, 3, 4, 5}) t.insert(v, v);
    auto it = t.find(3);
    REQUIRE(it != t.end());
    auto next = t.erase(it);
    REQUIRE(next != t.end());
    CHECK(next.key() == 4);
    CHECK_FALSE(t.contains(3));
    CHECK(t.validate());
}

TEST_CASE("non-default-constructible Value compiles and works") {
    struct NoDefault {
        int x;
        explicit NoDefault(int v) : x(v) {}
    };
    RedBlackTree<int, NoDefault> t;
    t.insert(1, NoDefault(10));
    t.insert(2, NoDefault(20));
    CHECK(t.at(1).x == 10);
    CHECK(t.at(2).x == 20);
    CHECK(t.validate());
}
