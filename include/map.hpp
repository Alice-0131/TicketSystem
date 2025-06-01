#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less <Key>
> class map {
 public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;
  struct AvlNode {
    value_type data;
    AvlNode *left;
    AvlNode *right;
    AvlNode *parent;
    int height;

    AvlNode(const value_type &element, AvlNode *lt, AvlNode *rt, AvlNode *pt, int h = 1)
    :data(element), left(lt), right(rt), parent(pt), height(h){}
  };
  class iterator {
    friend class map;
    friend class const_iterator;
   private:
    AvlNode *p;
    map *tree;
   public:
    iterator():p(nullptr), tree(nullptr){}

    iterator(const iterator &other):p(other.p), tree(other.tree){}

    iterator(AvlNode *p_, map *tree_): p(p_), tree(tree_){}

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator &operator++() {
      if (!p) {
        throw invalid_iterator();
      }
      if (p->right) { // p有右儿子
        p = p->right;
        while (p->left) {
          p = p->left;
        }
      } else if (p->parent) { // p无右儿子，但有父亲
        AvlNode *tmp = p;
        while (tmp->parent && tmp->parent->right == tmp) {
          tmp = tmp->parent;
        }
        p = tmp->parent;
      } else { // p无右儿子，也无父亲
        p = nullptr;
      }
      return *this;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    iterator &operator--() {
      if (!p) {
        p = tree->root;
        if (!p) {
          throw invalid_iterator();
        }
        while (p->right) {
          p = p->right;
        }
        return *this;
      }
      if (p->left) { // p有左儿子
        p = p->left;
        while (p->right) {
          p = p->right;
        }
      } else if (p->parent) { // p无左儿子，但有父亲
        AvlNode *tmp = p;
        while (tmp->parent && tmp->parent->left == tmp) {
          tmp = tmp->parent;
        }
        if (tmp->parent) {
          p = tmp->parent;
        } else {
          throw invalid_iterator();
        }
      } else {// p无左儿子，也无父亲
        throw invalid_iterator();
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
      if (!p) {
        throw invalid_iterator();
      }
      return p->data;
    }

    bool operator==(const iterator &rhs) const {
      return (p == rhs.p && tree == rhs.tree);
    }

    bool operator==(const const_iterator &rhs) const {
      return (p == rhs.p && tree == rhs.tree);
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const
    noexcept {
      if (!p) {
        throw invalid_iterator();
      }
      return &p->data;
    }
  };
  class const_iterator {
    friend class map;
    friend class iterator;
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
   private:
    // data members.
    const AvlNode *p;
    const map *tree;
   public:
    const_iterator():p(nullptr), tree(nullptr) {}

    const_iterator(const const_iterator &other): p(other.p), tree(other.tree) {}

    const_iterator(const iterator &other): p(other.p), tree(other.tree) {}

    const_iterator(const AvlNode *p_, const map *tree_): p(p_), tree(tree_){}

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    const_iterator &operator++() {
      if (!p) {
        throw invalid_iterator();
      }
      if (p->right) { // p有右儿子
        p = p->right;
        while (p->left) {
          p = p->left;
        }
      } else if (p->parent) { // p无右儿子，但有父亲
        const AvlNode *tmp = p;
        while (tmp->parent && tmp->parent->right == tmp) {
          tmp = tmp->parent;
        }
        p = tmp->parent;
      } else { // p无右儿子，也无父亲
        p = nullptr;
      }
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    const_iterator &operator--() {
      if (!p) {
        p = tree->root;
        if (!p) {
          throw invalid_iterator();
        }
        while (p->right) {
          p = p->right;
        }
        return *this;
      }
      if (p->left) { // p有左儿子
        p = p->left;
        while (p->right) {
          p = p->right;
        }
      } else if (p->parent) { // p无左儿子，但有父亲
        const AvlNode *tmp = p;
        while (tmp->parent && tmp->parent->left == tmp) {
          tmp = tmp->parent;
        }
        if (tmp->parent) {
          p = tmp->parent;
        } else {
          throw invalid_iterator();
        }
      } else {// p无左儿子，也无父亲
        throw invalid_iterator();
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    const value_type &operator*() const {
      if (!p) {
        throw invalid_iterator();
      }
      return p->data;
    }

    bool operator==(const iterator &rhs) const {
      return (p == rhs.p && tree == rhs.tree);
    }

    bool operator==(const const_iterator &rhs) const {
      return (p == rhs.p && tree == rhs.tree);
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }

    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    const value_type *operator->() const
    noexcept {
      if (!p) {
        throw invalid_iterator();
      }
      return &p->data;
    }
  };

  map(): root(nullptr), size_(0){}

  map(const map &other) {
    buildTree(root, other.root, nullptr);
    size_ = other.size_;
  }

  map &operator=(const map &other) {
    if (this == &other) {
      return *this;
    }
    clear();
    buildTree(root, other.root, nullptr);
    size_ = other.size_;
    return *this;
  }

  ~map() {
    clear();
  }

  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    AvlNode *t = find(key).p;
    if (t == nullptr) {
      throw index_out_of_bound();
    } else {
      return t->data.second;
    }
  }

  const T &at(const Key &key) const {
    const AvlNode *t = find(key).p;
    if (t == nullptr) {
      throw index_out_of_bound();
    } else {
      return t->data.second;
    }
  }

  /**
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    AvlNode *t = find(key).p;
    if (t == nullptr) {
      value_type new_data(key, T());
      t = insert(new_data).first.p;
    }
    return t->data.second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    const AvlNode *t = find(key).p;
    if (t == nullptr) {
      throw index_out_of_bound();
    } else {
      return t->data.second;
    }
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    AvlNode *t = root;
    if (t) {
      while (t->left) {
        t = t->left;
      }
    }
    return iterator(t, this);
  }

  const_iterator cbegin() const {
    const AvlNode *t = root;
    if (t) {
      while (t->left) {
        t = t->left;
      }
    }
    return const_iterator(t, this);
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    return iterator(nullptr, this);
  }

  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
    return size_;
  }

  /**
   * clears the contents
   */
  void clear() {
    makeEmpty(root);
    size_ = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    bool is_succeed = true;
    AvlNode *node = insert(value, root, nullptr, is_succeed);
    iterator it(node, this);
    if (is_succeed) {
      ++size_;
    }
    return {it, is_succeed};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if (pos == end() || pos.tree != this) {
      throw invalid_iterator();
    }
    remove(pos.p->data.first, root);
    --size_;
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    const AvlNode *t = find(key).p;
    if (t == nullptr) {
      return 0;
    } else {
      return 1;
    }
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    AvlNode *t = root;
    while (t != nullptr && (compare_(t->data.first, key) || compare_(key, t->data.first))) {
      if (compare_(key, t->data.first)) {
        t = t->left;
      } else {
        t = t->right;
      }
    }
    if (t == nullptr) {
      return end();
    } else {
      return iterator(t, this);
    }
  }

  const_iterator find(const Key &key) const {
    AvlNode *t = root;
    while (t != nullptr && (compare_(t->data.first, key) || compare_(key, t->data.first))) {
      if (compare_(key, t->data.first)) {
        t = t->left;
      } else {
        t = t->right;
      }
    }
    if (t == nullptr) {
      return cend();
    } else {
      return const_iterator(t, this);
    }
  }

 private:
  AvlNode *root;
  size_t size_;
  Compare compare_;

  AvlNode *insert(const value_type &x, AvlNode *&t, AvlNode *parent, bool &is_succeed) {
    AvlNode *return_value;
    if (t == nullptr) {
      t = new AvlNode(x, nullptr, nullptr, parent);
      return_value = t;
    } else if (compare_(x.first, t->data.first)) {
      return_value = insert(x, t->left, t, is_succeed);
      if (height(t->left) - height(t->right) == 2) {
        if (compare_(x.first, t->left->data.first)) {
          LL(t);
        } else {
          LR(t);
        }
      }
    } else if (compare_(t->data.first, x.first)) {
      return_value = insert(x, t->right, t, is_succeed);
      if (height(t->right) - height(t->left) == 2) {
        if (compare_(t->right->data.first, x.first)) {
          RR(t);
        } else {
          RL(t);
        }
      }
    } else {
      return_value = t;
      is_succeed = false;
    }
    t->height = max(height(t->left), height(t->right)) + 1;
    return return_value;
  }

  bool remove(const Key &x, AvlNode *&t) {
    if (!t) {
      return true;
    }
    if (compare_(x, t->data.first)) { // 在左子树上删除
      if (remove(x, t->left)) {
        return true;
      }
      return adjust(t, 0);
    } else if (compare_(t->data.first, x)) { // 在右子树上删除
      if (remove(x, t->right)) {
        return true;
      }
      return adjust(t, 1);
    } else { // 删除根节点
      if (t->left == nullptr || t->right == nullptr) { // 被删节点是叶节点或只有一个儿子
        AvlNode *old_node = t;
        t = (t->left != nullptr) ? t->left : t->right;
        if (t) {
          t->parent = old_node->parent;
        }
        delete old_node;
        return false;
      } else { // 被删节点有两个儿子
        AvlNode *tmp = t->right;
        while (tmp->left) {
          tmp = tmp->left;
        }
        if (t->right == tmp) {
          AvlNode *tmp_right = tmp->right;
          int tmp_height = tmp->height;
          // 给tmp赋值
          tmp->parent = t->parent;
          tmp->left = t->left;
          tmp->right = t;
          tmp->height = t->height;
          // 让别的指针指向tmp（tmp->right不需要写，因为tmp->right == t）
          tmp->left->parent = tmp;
          // 给t赋值
          t->parent = tmp;
          t->left = nullptr;
          t->right = tmp_right;
          t->height = tmp_height;
          // 让别的指针指向t
          if (t->right) {
            t->right->parent = t;
          }
          t = tmp;
        } else {
          AvlNode *tmp_parent = tmp->parent;
          AvlNode *tmp_right = tmp->right;
          int tmp_height = tmp->height;
          // 给tmp赋值
          tmp->parent = t->parent;
          tmp->left = t->left;
          tmp->right = t->right;
          tmp->height = t->height;
          // 让别的指针指向tmp
          tmp->left->parent = tmp;
          tmp->right->parent = tmp;
          // 给t赋值
          t->parent = tmp_parent;
          t->left = nullptr;
          t->right = tmp_right;
          t->height = tmp_height;
          // 让别的指针指向t
          if (t->right) {
            t->right->parent = t;
          }
          t->parent->left = t;
          t = tmp;
        }
        if (remove(x, t->right)) {
          return true;
        }
        return adjust(t, 1);
      }
    }
  }

  void makeEmpty(AvlNode *&t) {
    if (t == nullptr) {
      return;
    }
    makeEmpty(t->left);
    makeEmpty(t->right);
    delete t;
    t = nullptr;
  }

  int height(AvlNode *t)const {
    return t == nullptr ? 0 : t->height;
  }

  void LL(AvlNode *&t) {
    AvlNode *t1 = t->left;
    t->left = t1->right;
    t1->right = t;
    t->height = max(height(t->left), height(t->right)) + 1;
    t1->height = max(height(t1->left), height(t1->right)) + 1;
    if (t->left) {
      t->left->parent = t;
    }
    t1->parent = t->parent;
    t->parent = t1;
    t = t1;
  }

  void LR(AvlNode *&t) {
    RR(t->left);
    LL(t);
  }

  void RL(AvlNode *&t) {
    LL(t->right);
    RR(t);
  }

  void RR(AvlNode *&t) {
    AvlNode *t1 = t->right;
    t->right = t1->left;
    t1->left = t;
    t->height = max(height(t->left), height(t->right)) + 1;
    t1->height = max(height(t1->left), height(t1->right)) + 1;
    if (t->right) {
      t->right->parent = t;
    }
    t1->parent = t->parent;
    t->parent = t1;
    t = t1;
  }

  int max(int a, int b) {
    return (a > b) ? a : b;
  }

  bool adjust(AvlNode *&t, int subTree) {
    if (subTree) { // 在右子树上删除
      if (height(t->left) - height(t->right) == 1) {
        return true;
      } else if (height(t->right) == height(t->left)) {
        --t->height;
        return false;
      } else if (height(t->left->right) > height(t->left->left)) {
        LR(t);
        return false;
      } else {
        LL(t);
        if (height(t->right) == height(t->left)) {
          return false;
        } else {
          return true;
        }
      }
    } else { // 在左子树上删除
      if (height(t->right) - height(t->left) == 1) {
        return true;
      } else if (height(t->right) == height(t->left)) {
        --t->height;
        return false;
      } else if (height(t->right->left) > height(t->right->right)) {
        RL(t);
        return false;
      } else {
        RR(t);
        if (height(t->right) == height(t->left)) {
          return false;
        } else {
          return true;
        }
      }
    }
  }

  void buildTree(AvlNode *&new_node, AvlNode *old_node, AvlNode *parent) {
    if (!old_node) {
      new_node = nullptr;
      return;
    }
    new_node = new AvlNode(old_node->data, nullptr, nullptr, parent, old_node->height);
    buildTree(new_node->left, old_node->left, new_node);
    buildTree(new_node->right, old_node->right, new_node);
  }
};

}

#endif