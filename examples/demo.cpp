// SPDX-License-Identifier: MIT
//
// Walk-through of the public RedBlackTree API.

#include "rbtree/red_black_tree.hpp"

#include <iostream>
#include <string>

int main() {
    rbtree::RedBlackTree<int, std::string> tree;

    for (auto [k, v] : {std::pair{10, "Ten"}, {20, "Twenty"}, {30, "Thirty"},
                        {15, "Fifteen"}, {25, "Twenty-Five"}, {5, "Five"},
                        {1, "One"}}) {
        tree.insert(k, v);
    }

    std::cout << "size: " << tree.size() << '\n';

    std::cout << "in-order: ";
    tree.in_order([](const int& k, const std::string&) { std::cout << k << ' '; });
    std::cout << '\n';

    if (auto it = tree.find(15); it != tree.end()) {
        std::cout << "found 15 -> " << it.value() << '\n';
    }

    std::cout << "tree valid: " << std::boolalpha << tree.validate() << '\n';

    tree.erase(20);
    std::cout << "after erasing 20, in-order: ";
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        std::cout << it.key() << ' ';
    }
    std::cout << '\n';

    std::cout << "pre-order: ";
    tree.pre_order([](const int& k, const std::string&) { std::cout << k << ' '; });
    std::cout << '\n';

    std::cout << "post-order: ";
    tree.post_order([](const int& k, const std::string&) { std::cout << k << ' '; });
    std::cout << '\n';
}
