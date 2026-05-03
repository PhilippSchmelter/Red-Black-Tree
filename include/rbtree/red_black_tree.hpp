// SPDX-License-Identifier: MIT
//
// Red-Black Tree — a header-only, templated, self-balancing BST.
//
// Design notes
// ------------
// * NIL sentinel: every "null" child points to a single shared sentinel node
//   coloured BLACK. This removes the bulk of null-checks from the fix-up
//   routines and matches the canonical CLRS presentation.
// * Iterative destroy: cleanup uses an explicit traversal so very large trees
//   cannot blow the call stack.
// * Template visitors: traversals take the visitor by value so the call site
//   is inlined — no `std::function` indirection.
// * Const-only iterators: keys are immutable (their order is the invariant);
//   values may be mutated via `at()`.

#ifndef RBTREE_RED_BLACK_TREE_HPP
#define RBTREE_RED_BLACK_TREE_HPP

#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace rbtree {

template <typename Key, typename Value = Key, typename Compare = std::less<Key>>
class RedBlackTree {
public:
    enum class Color : unsigned char { Red, Black };

    using key_type       = Key;
    using mapped_type    = Value;
    using key_compare    = Compare;
    using size_type      = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    struct Node {
        Key   key;
        Value value;
        Color color;
        Node* parent;
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v, Color c, Node* nil) noexcept(
            std::is_nothrow_copy_constructible_v<Key> &&
            std::is_nothrow_copy_constructible_v<Value>)
            : key(k), value(v), color(c), parent(nil), left(nil), right(nil) {}

        Node(Key&& k, Value&& v, Color c, Node* nil) noexcept(
            std::is_nothrow_move_constructible_v<Key> &&
            std::is_nothrow_move_constructible_v<Value>)
            : key(std::move(k)), value(std::move(v)), color(c),
              parent(nil), left(nil), right(nil) {}
    };

public:
    // -------------------------------------------------------------------
    // Iterator
    // -------------------------------------------------------------------
    class const_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::pair<const Key&, const Value&>;
        using difference_type   = std::ptrdiff_t;
        using reference         = value_type;
        using pointer           = void;

        const_iterator() noexcept = default;

        reference operator*() const noexcept { return {node_->key, node_->value}; }

        const Key&   key()   const noexcept { return node_->key; }
        const Value& value() const noexcept { return node_->value; }

        const_iterator& operator++() noexcept {
            node_ = tree_->successor_of(node_);
            return *this;
        }
        const_iterator operator++(int) noexcept {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        const_iterator& operator--() noexcept {
            node_ = tree_->predecessor_of(node_);
            return *this;
        }
        const_iterator operator--(int) noexcept {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b) noexcept {
            return a.node_ == b.node_;
        }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) noexcept {
            return a.node_ != b.node_;
        }

    private:
        friend class RedBlackTree;
        const_iterator(Node* n, const RedBlackTree* t) noexcept : node_(n), tree_(t) {}
        Node*               node_ = nullptr;
        const RedBlackTree* tree_ = nullptr;
    };

    using iterator = const_iterator;

    // -------------------------------------------------------------------
    // Construction / destruction / assignment
    // -------------------------------------------------------------------
    RedBlackTree() : RedBlackTree(Compare{}) {}

    explicit RedBlackTree(const Compare& comp)
        : nil_(make_nil()), root_(nil_), size_(0), comp_(comp) {}

    RedBlackTree(const RedBlackTree& other)
        : nil_(make_nil()), root_(nil_), size_(0), comp_(other.comp_) {
        try {
            root_ = clone(other.root_, nil_, other.nil_);
            size_ = other.size_;
        } catch (...) {
            destroy_iterative(root_);
            destroy_nil(nil_);
            throw;
        }
    }

    RedBlackTree(RedBlackTree&& other) noexcept
        : nil_(other.nil_), root_(other.root_), size_(other.size_),
          comp_(std::move(other.comp_)) {
        other.nil_  = make_nil();
        other.root_ = other.nil_;
        other.size_ = 0;
    }

    RedBlackTree& operator=(const RedBlackTree& other) {
        if (this != &other) {
            RedBlackTree tmp(other);
            swap(tmp);
        }
        return *this;
    }

    RedBlackTree& operator=(RedBlackTree&& other) noexcept {
        if (this != &other) {
            clear();
            destroy_nil(nil_);
            nil_  = other.nil_;
            root_ = other.root_;
            size_ = other.size_;
            comp_ = std::move(other.comp_);
            other.nil_  = make_nil();
            other.root_ = other.nil_;
            other.size_ = 0;
        }
        return *this;
    }

    ~RedBlackTree() {
        destroy_iterative(root_);
        destroy_nil(nil_);
    }

    void swap(RedBlackTree& other) noexcept {
        using std::swap;
        swap(nil_,  other.nil_);
        swap(root_, other.root_);
        swap(size_, other.size_);
        swap(comp_, other.comp_);
    }

    friend void swap(RedBlackTree& a, RedBlackTree& b) noexcept { a.swap(b); }

    // -------------------------------------------------------------------
    // Capacity
    // -------------------------------------------------------------------
    [[nodiscard]] bool      empty() const noexcept { return size_ == 0; }
    [[nodiscard]] size_type size()  const noexcept { return size_; }

    // -------------------------------------------------------------------
    // Lookup
    // -------------------------------------------------------------------
    [[nodiscard]] iterator find(const Key& key) const noexcept {
        Node* n = find_node(key);
        return n == nil_ ? end() : iterator(n, this);
    }

    [[nodiscard]] bool contains(const Key& key) const noexcept {
        return find_node(key) != nil_;
    }

    [[nodiscard]] const Value& at(const Key& key) const {
        Node* n = find_node(key);
        if (n == nil_) throw std::out_of_range("RedBlackTree::at: key not found");
        return n->value;
    }

    [[nodiscard]] Value& at(const Key& key) {
        Node* n = find_node(key);
        if (n == nil_) throw std::out_of_range("RedBlackTree::at: key not found");
        return n->value;
    }

    // -------------------------------------------------------------------
    // Iteration
    // -------------------------------------------------------------------
    [[nodiscard]] iterator begin() const noexcept {
        return iterator(min_subtree(root_), this);
    }

    [[nodiscard]] iterator end() const noexcept { return iterator(nil_, this); }

    // -------------------------------------------------------------------
    // Modifiers
    // -------------------------------------------------------------------
    std::pair<iterator, bool> insert(const Key& key, const Value& value) {
        return insert_impl(key, value);
    }

    std::pair<iterator, bool> insert(Key&& key, Value&& value) {
        return insert_impl(std::move(key), std::move(value));
    }

    bool erase(const Key& key) {
        Node* z = find_node(key);
        if (z == nil_) return false;
        erase_node(z);
        return true;
    }

    iterator erase(iterator it) {
        if (it == end()) return end();
        Node* z    = it.node_;
        Node* succ = successor_of(z);
        erase_node(z);
        return iterator(succ, this);
    }

    void clear() noexcept {
        destroy_iterative(root_);
        root_ = nil_;
        size_ = 0;
    }

    // -------------------------------------------------------------------
    // Traversal — visitor takes (const Key&, const Value&)
    // -------------------------------------------------------------------
    template <typename Visitor>
    void in_order(Visitor visit) const {
        in_order_impl(root_, visit);
    }

    template <typename Visitor>
    void pre_order(Visitor visit) const {
        pre_order_impl(root_, visit);
    }

    template <typename Visitor>
    void post_order(Visitor visit) const {
        post_order_impl(root_, visit);
    }

    // -------------------------------------------------------------------
    // Validation — verifies all five Red-Black invariants. O(n).
    // -------------------------------------------------------------------
    [[nodiscard]] bool validate() const noexcept {
        if (nil_->color != Color::Black) return false;       // (3)
        if (root_ == nil_) return true;
        if (root_->color != Color::Black) return false;      // (2)
        int expected = -1;
        return validate_impl(root_, 0, expected);            // (1,4,5) + BST
    }

private:
    // -------------------------------------------------------------------
    // Internal helpers
    // -------------------------------------------------------------------
    // The NIL sentinel only ever uses its color/parent/left/right slots; its
    // Key/Value subobjects are deliberately left uninitialised so that this
    // template can be instantiated with non-default-constructible types.
    // Hence we hand-allocate the storage and bypass Node's ctor/dtor here.
    static Node* make_nil() {
        void* mem = ::operator new(sizeof(Node));
        Node* n   = static_cast<Node*>(mem);
        n->color  = Color::Black;
        n->parent = n;
        n->left   = n;
        n->right  = n;
        return n;
    }

    static void destroy_nil(Node* nil) noexcept {
        ::operator delete(static_cast<void*>(nil));
    }

    Node* min_subtree(Node* n) const noexcept {
        if (n == nil_) return nil_;
        while (n->left != nil_) n = n->left;
        return n;
    }

    Node* max_subtree(Node* n) const noexcept {
        if (n == nil_) return nil_;
        while (n->right != nil_) n = n->right;
        return n;
    }

    Node* successor_of(Node* n) const noexcept {
        if (n == nil_) return nil_;
        if (n->right != nil_) return min_subtree(n->right);
        Node* p = n->parent;
        while (p != nil_ && n == p->right) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    // operator-- on end() must yield the largest node, so map nil_ -> max.
    Node* predecessor_of(Node* n) const noexcept {
        if (n == nil_) return max_subtree(root_);
        if (n->left != nil_) return max_subtree(n->left);
        Node* p = n->parent;
        while (p != nil_ && n == p->left) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    Node* find_node(const Key& key) const noexcept {
        Node* x = root_;
        while (x != nil_) {
            if (comp_(key, x->key))      x = x->left;
            else if (comp_(x->key, key)) x = x->right;
            else                         return x;
        }
        return nil_;
    }

    template <typename K, typename V>
    std::pair<iterator, bool> insert_impl(K&& key, V&& value) {
        Node* y = nil_;
        Node* x = root_;
        while (x != nil_) {
            y = x;
            if (comp_(key, x->key))      x = x->left;
            else if (comp_(x->key, key)) x = x->right;
            else                         return {iterator(x, this), false};
        }
        Node* z = new Node(std::forward<K>(key), std::forward<V>(value),
                           Color::Red, nil_);
        z->parent = y;
        if (y == nil_)                  root_   = z;
        else if (comp_(z->key, y->key)) y->left = z;
        else                            y->right = z;
        insert_fixup(z);
        ++size_;
        return {iterator(z, this), true};
    }

    void rotate_left(Node* x) noexcept {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != nil_) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_)         root_ = y;
        else if (x == x->parent->left) x->parent->left = y;
        else                           x->parent->right = y;
        y->left   = x;
        x->parent = y;
    }

    void rotate_right(Node* x) noexcept {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nil_) y->right->parent = x;
        y->parent = x->parent;
        if (x->parent == nil_)          root_ = y;
        else if (x == x->parent->right) x->parent->right = y;
        else                            x->parent->left = y;
        y->right  = x;
        x->parent = y;
    }

    void insert_fixup(Node* z) noexcept {
        while (z->parent->color == Color::Red) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y->color == Color::Red) {
                    z->parent->color         = Color::Black;
                    y->color                 = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        rotate_left(z);
                    }
                    z->parent->color         = Color::Black;
                    z->parent->parent->color = Color::Red;
                    rotate_right(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y->color == Color::Red) {
                    z->parent->color         = Color::Black;
                    y->color                 = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rotate_right(z);
                    }
                    z->parent->color         = Color::Black;
                    z->parent->parent->color = Color::Red;
                    rotate_left(z->parent->parent);
                }
            }
        }
        root_->color = Color::Black;
    }

    void transplant(Node* u, Node* v) noexcept {
        if (u->parent == nil_)         root_ = v;
        else if (u == u->parent->left) u->parent->left = v;
        else                           u->parent->right = v;
        v->parent = u->parent;
    }

    void erase_node(Node* z) {
        Node* y               = z;
        Color y_original_color = y->color;
        Node* x;
        if (z->left == nil_) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == nil_) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y                = min_subtree(z->right);
            y_original_color = y->color;
            x                = y->right;
            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right         = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left         = z->left;
            y->left->parent = y;
            y->color        = z->color;
        }
        delete z;
        --size_;
        if (y_original_color == Color::Black) erase_fixup(x);
    }

    void erase_fixup(Node* x) noexcept {
        while (x != root_ && x->color == Color::Black) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == Color::Red) {
                    w->color         = Color::Black;
                    x->parent->color = Color::Red;
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == Color::Black && w->right->color == Color::Black) {
                    w->color = Color::Red;
                    x = x->parent;
                } else {
                    if (w->right->color == Color::Black) {
                        w->left->color = Color::Black;
                        w->color       = Color::Red;
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->color         = x->parent->color;
                    x->parent->color = Color::Black;
                    w->right->color  = Color::Black;
                    rotate_left(x->parent);
                    x = root_;
                }
            } else {
                Node* w = x->parent->left;
                if (w->color == Color::Red) {
                    w->color         = Color::Black;
                    x->parent->color = Color::Red;
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == Color::Black && w->left->color == Color::Black) {
                    w->color = Color::Red;
                    x = x->parent;
                } else {
                    if (w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color        = Color::Red;
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->color         = x->parent->color;
                    x->parent->color = Color::Black;
                    w->left->color   = Color::Black;
                    rotate_right(x->parent);
                    x = root_;
                }
            }
        }
        x->color = Color::Black;
    }

    void destroy_iterative(Node* root) noexcept {
        // Morris-style left-rotation walk would be cuter, but a parent-pointer
        // post-order walk is simpler and still O(n) without recursion.
        Node* n = root;
        while (n != nil_) {
            if (n->left != nil_) {
                n = n->left;
            } else if (n->right != nil_) {
                n = n->right;
            } else {
                Node* parent = n->parent;
                if (parent != nil_) {
                    if (parent->left == n) parent->left = nil_;
                    else                   parent->right = nil_;
                }
                delete n;
                n = parent;
            }
        }
    }

    Node* clone(Node* src, Node* parent, Node* src_nil) {
        if (src == src_nil) return nil_;
        Node* n   = new Node(src->key, src->value, src->color, nil_);
        n->parent = parent;
        try {
            n->left = clone(src->left, n, src_nil);
            try {
                n->right = clone(src->right, n, src_nil);
            } catch (...) {
                destroy_iterative(n->left);
                throw;
            }
        } catch (...) {
            delete n;
            throw;
        }
        return n;
    }

    template <typename Visitor>
    void in_order_impl(Node* n, Visitor& visit) const {
        if (n == nil_) return;
        in_order_impl(n->left, visit);
        visit(static_cast<const Key&>(n->key), static_cast<const Value&>(n->value));
        in_order_impl(n->right, visit);
    }

    template <typename Visitor>
    void pre_order_impl(Node* n, Visitor& visit) const {
        if (n == nil_) return;
        visit(static_cast<const Key&>(n->key), static_cast<const Value&>(n->value));
        pre_order_impl(n->left, visit);
        pre_order_impl(n->right, visit);
    }

    template <typename Visitor>
    void post_order_impl(Node* n, Visitor& visit) const {
        if (n == nil_) return;
        post_order_impl(n->left, visit);
        post_order_impl(n->right, visit);
        visit(static_cast<const Key&>(n->key), static_cast<const Value&>(n->value));
    }

    bool validate_impl(Node* n, int black_count, int& expected) const noexcept {
        if (n == nil_) {
            ++black_count; // the nil sentinel counts as a black leaf
            if (expected == -1) { expected = black_count; return true; }
            return black_count == expected;
        }
        if (n->left  != nil_ && comp_(n->key, n->left->key))  return false;
        if (n->right != nil_ && comp_(n->right->key, n->key)) return false;
        if (n->color == Color::Red) {
            if (n->left->color  == Color::Red) return false;
            if (n->right->color == Color::Red) return false;
        } else {
            ++black_count;
        }
        return validate_impl(n->left,  black_count, expected) &&
               validate_impl(n->right, black_count, expected);
    }

    Node*     nil_;
    Node*     root_;
    size_type size_;
    Compare   comp_;
};

} // namespace rbtree

#endif // RBTREE_RED_BLACK_TREE_HPP
