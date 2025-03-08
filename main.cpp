#include "RedBlackTree.h"
#include <iostream>
#include <string>

int main() {
    // Create a Red-Black Tree that maps integers to strings.
    RedBlackTree<int, std::string> tree;

    // Insert several key-value pairs into the tree.
    try {
        tree.insert(10, "Ten");
        tree.insert(20, "Twenty");
        tree.insert(30, "Thirty");
        tree.insert(15, "Fifteen");
        tree.insert(25, "Twenty-Five");
        tree.insert(5, "Five");
        tree.insert(1, "One");
    } catch (const std::runtime_error& e) {
        std::cerr << "Insertion error: " << e.what() << std::endl;
    }

    // Display the tree using an in-order traversal (should print keys in ascending order).
    std::cout << "In-order traversal: ";
    tree.inOrderTraversal([](RedBlackTree<int, std::string>::Node* node) {
        std::cout << node->key << " ";
    });
    std::cout << std::endl;

    // Search for a specific key.
    int keyToSearch = 15;
    auto node = tree.search(keyToSearch);
    if (node != nullptr) {
        std::cout << "Found key " << keyToSearch << " with value: " << node->value << std::endl;
    } else {
        std::cout << "Key " << keyToSearch << " not found." << std::endl;
    }

    // Validate the tree properties.
    if (tree.validate())
        std::cout << "Tree is valid." << std::endl;
    else
        std::cout << "Tree is invalid!" << std::endl;

    // Delete a key from the tree.
    try {
        tree.remove(20);
        std::cout << "After deleting key 20, in-order traversal: ";
        tree.inOrderTraversal([](RedBlackTree<int, std::string>::Node* node) {
            std::cout << node->key << " ";
        });
        std::cout << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Deletion error: " << e.what() << std::endl;
    }

    // Demonstrate pre-order traversal.
    std::cout << "Pre-order traversal: ";
    tree.preOrderTraversal([](RedBlackTree<int, std::string>::Node* node) {
        std::cout << node->key << " ";
    });
    std::cout << std::endl;

    // Demonstrate post-order traversal.
    std::cout << "Post-order traversal: ";
    tree.postOrderTraversal([](RedBlackTree<int, std::string>::Node* node) {
        std::cout << node->key << " ";
    });
    std::cout << std::endl;

    return 0;
}
