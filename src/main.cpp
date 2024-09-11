#include <iostream>
#include "RBTree.h"

int main() {
    RBTree<int> tree;

    // Insert elements
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(15);
    tree.insert(25);
    tree.insert(5);
    tree.insert(1);

    std::cout << "Red-Black-Tree after insertion:" << std::endl;
    tree.printTree();

    // Search for elements
    int searchValue = 15;
    auto searchResult = tree.search(searchValue);
    if (searchResult) {
        std::cout << "Element " << searchValue << " found." << std::endl;
    } else {
        std::cout << "Element " << searchValue << " not found." << std::endl;
    }

    // Deletion of elements
    int deleteValue = 20;
    tree.remove(deleteValue);
    std::cout << "\nRed-Black-Tree after deletion of " << deleteValue << ":" << std::endl;
    tree.printTree();

    return 0;
}
