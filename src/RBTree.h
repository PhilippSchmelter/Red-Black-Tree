#ifndef RBTREE_H
#define RBTREE_H

#include <iostream>
#include <memory>

enum class Color { RED, BLACK };

template <typename T>
class RBTree {
private:
    struct Node {
        T data;
        Color color;
        std::shared_ptr<Node> left, right, parent;

        explicit Node(T data)
            : data(data), color(Color::RED), left(nullptr), right(nullptr), parent(nullptr) {}
    };

    using NodePtr = std::shared_ptr<Node>;

    NodePtr root;

    void leftRotate(NodePtr x) {
        NodePtr y = x->right;
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(NodePtr x) {
        NodePtr y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    void insertFixup(NodePtr z) {
        while (z->parent && z->parent->color == Color::RED) {
            if (z->parent == z->parent->parent->left) {
                NodePtr y = z->parent->parent->right;
                if (y && y->color == Color::RED) {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                NodePtr y = z->parent->parent->left;
                if (y && y->color == Color::RED) {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = Color::BLACK;
    }

    void transplant(NodePtr u, NodePtr v) {
        if (!u->parent)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v)
            v->parent = u->parent;
    }

    void removeFixup(NodePtr x) {
        while (x != root && (!x || x->color == Color::BLACK)) {
            if (x == x->parent->left) {
                NodePtr w = x->parent->right;
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if ((!w->left || w->left->color == Color::BLACK) &&
                    (!w->right || w->right->color == Color::BLACK)) {
                    w->color = Color::RED;
                    x = x->parent;
                } else {
                    if (!w->right || w->right->color == Color::BLACK) {
                        if (w->left)
                            w->left->color = Color::BLACK;
                        w->color = Color::RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    if (w->right)
                        w->right->color = Color::BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                NodePtr w = x->parent->left;
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if ((!w->left || w->left->color == Color::BLACK) &&
                    (!w->right || w->right->color == Color::BLACK)) {
                    w->color = Color::RED;
                    x = x->parent;
                } else {
                    if (!w->left || w->left->color == Color::BLACK) {
                        if (w->right)
                            w->right->color = Color::BLACK;
                        w->color = Color::RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    if (w->left)
                        w->left->color = Color::BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        if (x)
            x->color = Color::BLACK;
    }

    NodePtr minimum(NodePtr node) const {
        while (node->left)
            node = node->left;
        return node;
    }

    void remove(NodePtr z) {
        NodePtr y = z;
        NodePtr x;
        Color originalColor = y->color;
        if (!z->left) {
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            originalColor = y->color;
            x = y->right;
            if (y->parent == z) {
                if (x)
                    x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (originalColor == Color::BLACK)
            removeFixup(x);
    }

public:
    RBTree() : root(nullptr) {}

    void insert(T data) {
        NodePtr z = std::make_shared<Node>(data);
        NodePtr y = nullptr;
        NodePtr x = root;

        while (x) {
            y = x;
            if (z->data < x->data)
                x = x->left;
            else
                x = x->right;
        }

        z->parent = y;
        if (!y)
            root = z;
        else if (z->data < y->data)
            y->left = z;
        else
            y->right = z;

        insertFixup(z);
    }

    void remove(T data) {
        NodePtr z = root;
        while (z) {
            if (z->data == data) {
                remove(z);
                return;
            }
            if (data < z->data)
                z = z->left;
            else
                z = z->right;
        }
    }

    NodePtr search(T data) const {
        NodePtr node = root;
        while (node && node->data != data) {
            if (data < node->data)
                node = node->left;
            else
                node = node->right;
        }
        return node;
    }

    void print(NodePtr node, std::string indent, bool last) const {
        if (node) {
            std::cout << indent;
            if (last) {
                std::cout << "R----";
                indent += "   ";
            } else {
                std::cout << "L----";
                indent += "|  ";
            }
            std::string color = (node->color == Color::RED) ? "RED" : "BLACK";
            std::cout << node->data << "(" << color << ")" << std::endl;
            print(node->left, indent, false);
            print(node->right, indent, true);
        }
    }

    void printTree() const {
        if (root)
            print(root, "", true);
    }
};

#endif // RBTREE_H
