#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct node {
        T data;
        node *left, *right, *father;
        node(const T &data): data(data), left(nullptr), right(nullptr), father(nullptr){}
    };

    node* root;
    size_t currentSize;
    Compare cmp;

    void copy(node *&new_node, node *old_node, node *father) {
        if (!old_node) {
            new_node = nullptr;
            return;
        }
        new_node = new node(old_node->data);
        new_node->father = father;
        copy(new_node->left, old_node->left, new_node);
        copy(new_node->right, old_node->right, new_node);
    }
    void clear(node *&node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
            node = nullptr;
        }
    }
    node* merge(node* node1, node* node2) {
        if (!node1) {
            return node2;
        }
        if (!node2) {
            return node1;
        }
        if (cmp(node1->data, node2->data)) {
            std::swap(node1, node2);
        }
        node1->right = merge(node1->right, node2);
        std::swap(node1->left, node1->right);
        if (node1->left) {
            node1->left->father = node1;
        }
        if (node1->right) {
            node1->right->father = node1;
        }
        return node1;
    }

public:
    /**
     * @brief default constructor
     */
    priority_queue() {
        root = nullptr;
        currentSize = 0;
    }

    /**
     * @brief copy constructor
     * @param other the priority_queue to be copied
     */
    priority_queue(const priority_queue &other) {
        copy(root, other.root, nullptr);
        currentSize = other.currentSize;
    }

    /**
     * @brief deconstructor
     */
    ~priority_queue() {
        clear(root);
    }

    /**
     * @brief Assignment operator
     * @param other the priority_queue to be assigned from
     * @return a reference to this priority_queue after assignment
     */
    priority_queue &operator=(const priority_queue &other) {
        if (this == &other) {
            return *this;
        }
        clear(root);
        copy(root, other.root, nullptr);
        currentSize = other.currentSize;
        return *this;
    }

    /**
     * @brief get the top element of the priority queue.
     * @return a reference of the top element.
     * @throws container_is_empty if empty() returns true
     */
    const T & top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->data;
    }

    /**
     * @brief push new element to the priority queue.
     * @param e the element to be pushed
     */
    void push(const T &e) {
        node* other = new node(e);
        try {
            root = merge(root, other);
            ++currentSize;
        } catch (...) {
            delete other;
            throw;
        }
    }

    /**
     * @brief delete the top element from the priority queue.
     * @throws container_is_empty if empty() returns true
     */
    void pop() {
        if (empty()) {
            throw container_is_empty();
        }
        node* old_root = root;
        root = merge(root->left, root->right);
        if (root) {
            root->father = nullptr;
        }
        delete old_root;
        --currentSize;
    }

    /**
     * @brief return the number of elements in the priority queue.
     * @return the number of elements.
     */
    size_t size() const {
        return currentSize;
    }

    /**
     * @brief check if the container is empty.
     * @return true if it is empty, false otherwise.
     */
	bool empty() const {
        return currentSize == 0;
    }

    /**
     * @brief merge another priority_queue into this one.
     * The other priority_queue will be cleared after merging.
     * The complexity is at most O(logn).
     * @param other the priority_queue to be merged.
     */
    void merge(priority_queue &other) {
        root = merge(root, other.root);
        currentSize += other.currentSize;
        other.root = nullptr;
        other.currentSize = 0;
    }
};
}

#endif