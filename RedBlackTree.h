#ifndef REDBLACKTREE_H
#define REDBLACKTREE_H

#include <iostream>
#include <stdexcept>
#include <functional>
#include <utility>
#include <cassert>

/**
 * @brief A template-based Red-Black Tree implementation.
 *
 * This class implements a self-balancing binary search tree with the following properties:
 * 1. Every node is either red or black.
 * 2. The root is black.
 * 3. All leaves (nullptr) are black.
 * 4. If a node is red, then both its children are black.
 * 5. Every path from a node to its descendant leaves contains the same number of black nodes.
 *
 * The tree supports insertion, deletion, search, and different traversals.
 *
 * @tparam Key   Type used for keys.
 * @tparam Value Type used for values (defaults to Key for a simple set-like behavior).
 */
template<typename Key, typename Value = Key>
class RedBlackTree {
public:
    enum Color { RED, BLACK };

    /**
     * @brief Structure representing a node in the tree.
     */
    struct Node {
        Key key;         ///< Key associated with the node.
        Value value;     ///< Value associated with the key.
        Color color;     ///< Color of the node (RED or BLACK).
        Node* parent;    ///< Pointer to the parent node.
        Node* left;      ///< Pointer to the left child.
        Node* right;     ///< Pointer to the right child.

        /**
         * @brief Node constructor.
         * @param k The key.
         * @param v The value.
         * @param c Initial color (default is RED).
         * @param p Pointer to parent (default is nullptr).
         * @param l Pointer to left child (default is nullptr).
         * @param r Pointer to right child (default is nullptr).
         */
        Node(const Key& k, const Value& v, Color c = RED,
             Node* p = nullptr, Node* l = nullptr, Node* r = nullptr)
                : key(k), value(v), color(c), parent(p), left(l), right(r) {}
    };

private:
    Node* root;

public:
    /**
     * @brief Default constructor.
     *
     * Constructs an empty Red-Black Tree.
     */
    RedBlackTree() : root(nullptr) {}

    /**
     * @brief Destructor.
     *
     * Recursively deletes all nodes in the tree.
     */
    ~RedBlackTree() {
        destroy(root);
    }

    /**
     * @brief Copy constructor.
     *
     * Performs a deep copy of the given tree.
     * @param other The tree to copy.
     */
    RedBlackTree(const RedBlackTree& other) : root(nullptr) {
        root = clone(other.root, nullptr);
    }

    /**
     * @brief Copy assignment operator.
     *
     * Uses the copy-and-swap idiom for exception safety.
     * @param other The tree to copy.
     * @return Reference to this tree.
     */
    RedBlackTree& operator=(RedBlackTree other) {
        swap(other);
        return *this;
    }

    /**
     * @brief Inserts a new node with the given key and value.
     *
     * If the key already exists, an exception is thrown.
     *
     * @param key The key to insert.
     * @param value The value associated with the key.
     * @throws std::runtime_error if the key already exists.
     */
    void insert(const Key& key, const Value& value) {
        Node* newNode = new Node(key, value);
        Node* y = nullptr;
        Node* x = root;

        // Standard BST insertion to find the correct position.
        while (x != nullptr) {
            y = x;
            if (key < x->key)
                x = x->left;
            else if (key > x->key)
                x = x->right;
            else
                throw std::runtime_error("Duplicate key insertion is not allowed.");
        }

        newNode->parent = y;
        if (y == nullptr)
            root = newNode;
        else if (key < y->key)
            y->left = newNode;
        else
            y->right = newNode;

        insertFixup(newNode);
    }

    /**
     * @brief Deletes the node with the given key from the tree.
     *
     * If the key is not found, an exception is thrown.
     *
     * @param key The key to delete.
     * @throws std::runtime_error if the key is not found.
     */
    void remove(const Key& key) {
        Node* z = search(key);
        if (z == nullptr)
            throw std::runtime_error("Key not found in the tree.");

        Node* y = z;
        Node* x = nullptr;
        Color yOriginalColor = y->color;
        Node* fixupParent = nullptr;

        if (z->left == nullptr) {
            x = z->right;
            fixupParent = z->parent;
            transplant(z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            fixupParent = z->parent;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            fixupParent = (y->parent == z) ? y : y->parent;
            if (y->parent != z) {
                transplant(y, y->right);
                y->right = z->right;
                if (y->right)
                    y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            if (y->left)
                y->left->parent = y;
            y->color = z->color;
        }

        delete z;

        if (yOriginalColor == BLACK)
            deleteFixup(x, fixupParent);
    }

    /**
     * @brief Searches for a node with the given key.
     *
     * @param key The key to search for.
     * @return Pointer to the found node; nullptr if not found.
     */
    Node* search(const Key& key) const {
        Node* current = root;
        while (current != nullptr) {
            if (key < current->key)
                current = current->left;
            else if (key > current->key)
                current = current->right;
            else
                return current;
        }
        return nullptr;
    }

    /**
     * @brief Finds the node with the minimum key in the subtree rooted at 'node'.
     *
     * @param node Root of the subtree.
     * @return Pointer to the node with the minimum key; nullptr if node is nullptr.
     */
    Node* minimum(Node* node) const {
        if (node == nullptr) return nullptr;
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    /**
     * @brief Finds the node with the maximum key in the subtree rooted at 'node'.
     *
     * @param node Root of the subtree.
     * @return Pointer to the node with the maximum key; nullptr if node is nullptr.
     */
    Node* maximum(Node* node) const {
        if (node == nullptr) return nullptr;
        while (node->right != nullptr)
            node = node->right;
        return node;
    }

    /**
     * @brief Returns the in-order successor of the given node.
     *
     * @param node The node whose successor is to be found.
     * @return Pointer to the successor node; nullptr if none exists.
     */
    Node* successor(Node* node) const {
        if (node == nullptr)
            return nullptr;
        if (node->right != nullptr)
            return minimum(node->right);
        Node* y = node->parent;
        while (y != nullptr && node == y->right) {
            node = y;
            y = y->parent;
        }
        return y;
    }

    /**
     * @brief Returns the in-order predecessor of the given node.
     *
     * @param node The node whose predecessor is to be found.
     * @return Pointer to the predecessor node; nullptr if none exists.
     */
    Node* predecessor(Node* node) const {
        if (node == nullptr)
            return nullptr;
        if (node->left != nullptr)
            return maximum(node->left);
        Node* y = node->parent;
        while (y != nullptr && node == y->left) {
            node = y;
            y = y->parent;
        }
        return y;
    }

    /**
     * @brief In-order traversal.
     *
     * Visits nodes in ascending order (left-root-right). The provided function is called on each node.
     *
     * @param visit A function/lambda to process each visited node.
     */
    void inOrderTraversal(std::function<void(Node*)> visit) const {
        inOrderHelper(root, visit);
    }

    /**
     * @brief Pre-order traversal.
     *
     * Visits nodes in root-left-right order.
     *
     * @param visit A function/lambda to process each visited node.
     */
    void preOrderTraversal(std::function<void(Node*)> visit) const {
        preOrderHelper(root, visit);
    }

    /**
     * @brief Post-order traversal.
     *
     * Visits nodes in left-right-root order.
     *
     * @param visit A function/lambda to process each visited node.
     */
    void postOrderTraversal(std::function<void(Node*)> visit) const {
        postOrderHelper(root, visit);
    }

    /**
     * @brief Validates the Red-Black Tree properties.
     *
     * Checks that:
     * - The root is black.
     * - Red nodes have black children.
     * - Every path from a node to its descendant leaves has the same number of black nodes.
     *
     * @return true if the tree is valid; false otherwise.
     */
    bool validate() const {
        if (root == nullptr)
            return true;
        if (root->color != BLACK)
            return false;
        int blackCount = -1;
        return validateHelper(root, 0, blackCount);
    }

private:
    /**
     * @brief Helper for in-order traversal.
     *
     * @param node Current node.
     * @param visit Function to process the node.
     */
    void inOrderHelper(Node* node, std::function<void(Node*)> visit) const {
        if (node == nullptr) return;
        inOrderHelper(node->left, visit);
        visit(node);
        inOrderHelper(node->right, visit);
    }

    /**
     * @brief Helper for pre-order traversal.
     *
     * @param node Current node.
     * @param visit Function to process the node.
     */
    void preOrderHelper(Node* node, std::function<void(Node*)> visit) const {
        if (node == nullptr) return;
        visit(node);
        preOrderHelper(node->left, visit);
        preOrderHelper(node->right, visit);
    }

    /**
     * @brief Helper for post-order traversal.
     *
     * @param node Current node.
     * @param visit Function to process the node.
     */
    void postOrderHelper(Node* node, std::function<void(Node*)> visit) const {
        if (node == nullptr) return;
        postOrderHelper(node->left, visit);
        postOrderHelper(node->right, visit);
        visit(node);
    }

    /**
     * @brief Fixes the tree after insertion to restore Red-Black properties.
     *
     * @param z The newly inserted node.
     */
    void insertFixup(Node* z) {
        while (z != root && z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y && y->color == RED) {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    /**
     * @brief Fixes the tree after deletion to restore Red-Black properties.
     *
     * @param x The node that replaced the deleted node (may be nullptr).
     * @param parent The parent of x.
     */
    void deleteFixup(Node* x, Node* parent) {
        while ((x != root) && (x == nullptr || x->color == BLACK)) {
            if (x == (parent ? parent->left : nullptr)) {
                Node* w = parent->right;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    parent->color = RED;
                    leftRotate(parent);
                    w = parent->right;
                }
                if ( (w == nullptr) ||
                     ((w->left == nullptr || w->left->color == BLACK) &&
                      (w->right == nullptr || w->right->color == BLACK)) ) {
                    if (w) w->color = RED;
                    x = parent;
                    parent = x->parent;
                } else {
                    if (w->right == nullptr || w->right->color == BLACK) {
                        if (w->left)
                            w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = parent->right;
                    }
                    if(w) w->color = parent->color;
                    parent->color = BLACK;
                    if (w && w->right)
                        w->right->color = BLACK;
                    leftRotate(parent);
                    x = root;
                    break;
                }
            } else {
                Node* w = parent->left;
                if (w && w->color == RED) {
                    w->color = BLACK;
                    parent->color = RED;
                    rightRotate(parent);
                    w = parent->left;
                }
                if ((w == nullptr) ||
                    ((w->left == nullptr || w->left->color == BLACK) &&
                     (w->right == nullptr || w->right->color == BLACK))) {
                    if(w) w->color = RED;
                    x = parent;
                    parent = x->parent;
                } else {
                    if (w->left == nullptr || w->left->color == BLACK) {
                        if(w->right)
                            w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = parent->left;
                    }
                    if(w) w->color = parent->color;
                    parent->color = BLACK;
                    if (w && w->left)
                        w->left->color = BLACK;
                    rightRotate(parent);
                    x = root;
                    break;
                }
            }
        }
        if (x)
            x->color = BLACK;
    }

    /**
     * @brief Replaces subtree rooted at node u with subtree rooted at node v.
     *
     * @param u The root of the subtree to be replaced.
     * @param v The new subtree.
     */
    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v != nullptr)
            v->parent = u->parent;
    }

    /**
     * @brief Performs a left rotation at node x.
     *
     * @param x The node where the rotation occurs.
     */
    void leftRotate(Node* x) {
        Node* y = x->right;
        if (!y)
            return;
        x->right = y->left;
        if (y->left != nullptr)
            y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    /**
     * @brief Performs a right rotation at node y.
     *
     * @param y The node where the rotation occurs.
     */
    void rightRotate(Node* y) {
        Node* x = y->left;
        if (!x)
            return;
        y->left = x->right;
        if (x->right != nullptr)
            x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->right)
            y->parent->right = x;
        else
            y->parent->left = x;
        x->right = y;
        y->parent = x;
    }

    /**
     * @brief Recursively deletes all nodes in the subtree.
     *
     * @param node Root of the subtree.
     */
    void destroy(Node* node) {
        if (node != nullptr) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    /**
     * @brief Recursively clones the subtree.
     *
     * @param node The current node to clone.
     * @param parent The parent for the new cloned node.
     * @return Pointer to the cloned subtree's root.
     */
    Node* clone(Node* node, Node* parent) {
        if (node == nullptr)
            return nullptr;
        Node* newNode = new Node(node->key, node->value, node->color, parent);
        newNode->left = clone(node->left, newNode);
        newNode->right = clone(node->right, newNode);
        return newNode;
    }

    /**
     * @brief Swaps the contents of this tree with another.
     *
     * @param other The other tree.
     */
    void swap(RedBlackTree& other) {
        std::swap(root, other.root);
    }

    /**
     * @brief Helper function to validate Red-Black properties.
     *
     * Recursively checks that every path from the current node to a leaf has the same number
     * of black nodes and that red nodes do not have red children.
     *
     * @param node The current node.
     * @param blackCount Number of black nodes so far along the current path.
     * @param pathBlackCount The black count that every path should have (set on the first leaf).
     * @return true if valid; false otherwise.
     */
    bool validateHelper(Node* node, int blackCount, int& pathBlackCount) const {
        if (node == nullptr) {
            if (pathBlackCount == -1) {
                pathBlackCount = blackCount;
                return true;
            }
            return blackCount == pathBlackCount;
        }
        if (node->color == RED) {
            if ((node->left && node->left->color != BLACK) ||
                (node->right && node->right->color != BLACK))
                return false;
        }
        if (node->color == BLACK)
            blackCount++;
        return validateHelper(node->left, blackCount, pathBlackCount) &&
               validateHelper(node->right, blackCount, pathBlackCount);
    }
};

#endif // REDBLACKTREE_H
