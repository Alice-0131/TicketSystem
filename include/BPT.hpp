#ifndef BPT_HPP
#define BPT_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "MemoryRiver.hpp"
#include "vector.hpp"

inline long long Hash(std::string &s) {
  const long long N = 31;
  long long res = 0;
  for (int i = 0; i < s.length(); ++i) {
    res = res * N + s[i];
  }
  return res;
}

const int M = 50;
const int L = 50;

struct pair {
  long long index;
  int value;

  pair():index(0), value(0){}
  pair(long long i, int v): index(i), value(v){}
};

// 索引块
struct IndexBlock {
  int ptr;            // 此块在文件中的位置
  bool flag;          // 是否为叶节点
  int size;           // 关键字数量
  pair data[M + 5];  // 关键字
  int child[M + 5]{};// 子节点位置

  IndexBlock(int p = -1, bool f = true, int s = 0): ptr(p), flag(f), size(s){}
};

// 数据块
struct DataBlock {
  int ptr;          // 此块在文件中的位置
  int size;         // 存放的数据数量
  pair data[L + 5];// 存放的数据
  int nxt;          // 下一个数据块的位置

  DataBlock(int p = -1, int s = 1, int n = -1): ptr(p), size(s), nxt(n){}
};

class BPT {
private:
  MemoryRiver<IndexBlock> index_file;
  MemoryRiver<DataBlock> data_file;
  int index_root;
  int size_of_index;
  int size_of_data;

  int compare(pair a, pair b) {
    if (a.index < b.index) {
      return -1;
    }
    if (a.index > b.index) {
      return 1;
    }
    if (a.value < b.value) {
      return -1;
    }
    if (a.value == b.value) {
      return 0;
    }
    else {
      return 1;
    }
  }

  int upper_bound(pair a[], int size, pair target) { // 第一个大于target的下标
    int l = 0, r = size - 1;
    while (l <= r) {
      int mid = (l + r) / 2;
      if (compare(a[mid], target) <= 0) {
        l = mid + 1;
      } else {
        r = mid - 1;
      }
    }
    return l;
  }

  int lower_bound(pair a[], int size, long long target) { // 第一个大于等于target的下标
    int l = 0, r = size;
    while (l < r) {
      int mid = (l + r) / 2;
      if (a[mid].index < target) {
        l = mid + 1;
      } else {
        r = mid;
      }
    }
    return l;
  }

  bool insert(IndexBlock &cur, const pair &pair_) {
    int i = upper_bound(cur.data, cur.size, pair_); // pair_所在子节点在该块的下标
    if (cur.flag) { // cur是叶节点
      DataBlock data_block;
      data_file.read(data_block, cur.child[i]);
      int k = upper_bound(data_block.data, data_block.size, pair_); // pair_在数据块中的下标
      for (int j = data_block.size; j > k; --j) {
        data_block.data[j] = data_block.data[j - 1];
      }
      data_block.data[k] = pair_;
      data_block.size++;
      if (data_block.size > L) { // 裂块
        DataBlock new_block(size_of_data++, L - L / 2, data_block.nxt);
        for (int j = L / 2 + 1; j <= L; ++j) {
          new_block.data[j - L / 2 - 1] = data_block.data[j];
        }
        data_block.size = L / 2 + 1;
        data_block.nxt = new_block.ptr;
        data_file.write(data_block, data_block.ptr);
        data_file.write(new_block, new_block.ptr);
        pair inserted_pair = new_block.data[0];
        for (int j = cur.size; j > i; --j) {
          cur.data[j] = cur.data[j - 1];
        }
        cur.data[i] = inserted_pair;
        ++cur.size;
        for (int j = cur.size; j > i + 1; --j) {
          cur.child[j] = cur.child[j - 1];
        }
        cur.child[i + 1] = new_block.ptr;
        if (cur.size == M) {
          return true;
        }
        index_file.write(cur, cur.ptr);
      } else { // 直接插入
        data_file.write(data_block, data_block.ptr);
      }
    } else { // cur不是叶节点
      IndexBlock child;
      index_file.read(child, cur.child[i]);
      if (insert(child, pair_)) { // 子节点溢出了
        IndexBlock new_child(size_of_index++, child.flag, M - M / 2 - 1);
        for (int j = M / 2 + 1; j < M; ++j) {
          new_child.data[j - M / 2 - 1] = child.data[j];
        }
        for (int j = M / 2 + 1; j <= M; ++j) {
          new_child.child[j - M / 2 - 1] = child.child[j];
        }
        child.size = M / 2;
        pair inserted_pair = child.data[M / 2];
        i = upper_bound(cur.data, cur.size, inserted_pair);
        for (int j = cur.size; j > i; --j) {
          cur.data[j] = cur.data[j - 1];
        }
        cur.data[i] = inserted_pair;
        ++cur.size;
        for (int j = cur.size; j > i + 1; --j) {
          cur.child[j] = cur.child[j - 1];
        }
        cur.child[i + 1] = new_child.ptr;
        index_file.write(child, child.ptr);
        index_file.write(new_child, new_child.ptr);
        if (cur.size == M) {
          return true;
        }
        index_file.write(cur, cur.ptr);
      }
    }
    return false;
  }

  bool Delete(IndexBlock &cur, const pair &pair_) { // 在cur中删除索引ind，并删除索引后指向的数据块地址。若此次删除需要并块，则返回true
    int i = upper_bound(cur.data, cur.size, pair_); // pair_所在子节点的下标
    if (cur.flag) { // cur是叶节点
      DataBlock data_block;
      data_file.read(data_block, cur.child[i]);
      int k = upper_bound(data_block.data, data_block.size, pair_); // pair_在数据块中的下标
      if (k == 0) {
        return false;
      }
      --k;
      if (compare(data_block.data[k], pair_) != 0) {
        return false;
      }
      --data_block.size;
      for (int j = k; j < data_block.size; ++j) {
        data_block.data[j] = data_block.data[j + 1];
      }
      if (data_block.size < L / 2) { // 数据太少
        DataBlock right_block;
        DataBlock left_block;
        if (cur.size == 0) {
          data_file.write(data_block, data_block.ptr);
          return true;
        }
        if (i < cur.size) { // 考虑向右借
          data_file.read(right_block, cur.child[i + 1]);
          if (right_block.size > L / 2) { // 向右借孩子
            data_block.data[data_block.size++] = right_block.data[0];
            right_block.size--;
            for (int j = 0; j < right_block.size; ++j) {
              right_block.data[j] = right_block.data[j + 1];
            }
            cur.data[i] = right_block.data[0];
            index_file.write(cur, cur.ptr);
            data_file.write(data_block, data_block.ptr);
            data_file.write(right_block, right_block.ptr);
            return false;
          }
        }
        if (i > 0) { // 考虑向左借
          data_file.read(left_block, cur.child[i - 1]);
          if (left_block.size > L / 2) { // 向左借孩子
            for (int j = data_block.size; j > 0; --j) {
              data_block.data[j] = data_block.data[j - 1];
            }
            data_block.data[0] = left_block.data[--left_block.size];
            data_block.size++;
            cur.data[i - 1] = data_block.data[0];
            index_file.write(cur, cur.ptr);
            data_file.write(data_block, data_block.ptr);
            data_file.write(left_block, left_block.ptr);
            return false;
          }
        }
        if (i < cur.size) { // 向右并块
          for (int j = L / 2 - 1; j < L / 2 * 2 - 1; ++j) {
            data_block.data[j] = right_block.data[j - L / 2 + 1];
          }
          data_block.size = L / 2 * 2 - 1;
          data_block.nxt = right_block.nxt;
          data_file.write(data_block, data_block.ptr);
          for (int j = i; j < cur.size - 1; ++j) {
            cur.data[j] = cur.data[j + 1];
          }
          --cur.size;
          for (int j = i + 1; j <= cur.size; ++j) {
            cur.child[j] = cur.child[j + 1];
          }
          if (cur.size < (M - 1) / 2) {
            return true;
          }
          index_file.write(cur, cur.ptr);
        } else if (i > 0) { // 向左并块
          for (int j = L / 2; j < L / 2 * 2 - 1; ++j) {
            left_block.data[j] = data_block.data[j - L / 2];
          }
          left_block.size = L / 2 * 2 - 1;
          left_block.nxt = data_block.nxt;
          data_file.write(left_block, left_block.ptr);
          --i;
          for (int j = i; j < cur.size - 1; ++j) {
            cur.data[j] = cur.data[j + 1];
          }
          --cur.size;
          for (int j = i + 1; j <= cur.size; ++j) {
            cur.child[j] = cur.child[j + 1];
          }
          if (cur.size <(M - 1) / 2) {
            return true;
          }
          index_file.write(cur, cur.ptr);
        }
      } else { // 直接写入
        data_file.write(data_block, data_block.ptr);
      }
    } else { // cur不是叶节点
      IndexBlock child;
      index_file.read(child, cur.child[i]);
      if (Delete(child, pair_)) { // 子节点关键字数量太少
        if (cur.size == 0) {
          return true;
        }
        IndexBlock right_block;
        IndexBlock left_block;
        if (i < cur.size) { // 考虑向右借
          index_file.read(right_block, cur.child[i + 1]);
          if (right_block.size > (M - 1) / 2) { // 向右借孩子
            child.data[child.size++] = cur.data[i];
            child.child[child.size] = right_block.child[0];
            cur.data[i] = right_block.data[0];
            for (int j = 0; j < right_block.size; ++j) {
              right_block.child[j] = right_block.child[j + 1];
            }
            right_block.size--;
            for (int j = 0; j < right_block.size; ++j) {
              right_block.data[j] = right_block.data[j + 1];
            }
            index_file.write(cur, cur.ptr);
            index_file.write(child, child.ptr);
            index_file.write(right_block, right_block.ptr);
            return false;
          }
        }
        if (i > 0) { // 考虑向左借
          index_file.read(left_block, cur.child[i - 1]);
          if (left_block.size >= (M + 1) / 2) { // 向左借孩子
            for (int j = child.size; j > 0; --j) {
              child.data[j] = child.data[j - 1];
            }
            ++child.size;
            for (int j = child.size; j > 0; --j) {
              child.child[j] = child.child[j - 1];
            }
            child.data[0] = cur.data[i - 1];
            child.child[0] = left_block.child[left_block.size];
            cur.data[i - 1] = left_block.data[--left_block.size];
            index_file.write(left_block, left_block.ptr);
            index_file.write(child, child.ptr);
            index_file.write(cur, cur.ptr);
            return false;
          }
        }
        if (i < cur.size) { // 向右并块
          child.data[(M - 1) / 2 - 1] = cur.data[i];
          for (int j = (M - 1) / 2; j < (M - 1) / 2 * 2; ++j) {
            child.data[j] = right_block.data[j - (M - 1) / 2];
          }
          for (int j = (M - 1) / 2; j <= (M - 1) / 2 * 2; ++j) {
            child.child[j] = right_block.child[j - (M - 1) / 2];
          }
          child.size = (M - 1) / 2 * 2;
          for (int j = i; j < cur.size - 1; ++j) {
            cur.data[j] = cur.data[j + 1];
          }
          for (int j = i + 1; j < cur.size; ++j) {
            cur.child[j] = cur.child[j + 1];
          }
          cur.size--;
          index_file.write(child, child.ptr);
          if (cur.size < (M - 1) / 2) {
            return true;
          }
          index_file.write(cur, cur.ptr);
        } else if (i > 0) { // 向左并块
          left_block.data[(M - 1) / 2] = cur.data[i - 1];
          for (int j = (M - 1) / 2 + 1; j < (M - 1) / 2 * 2; ++j) {
            left_block.data[j] = child.data[j - (M - 1) / 2 - 1];
          }
          for (int j = (M - 1) / 2 + 1; j <= (M - 1) / 2 * 2; ++j) {
            left_block.child[j] = child.child[j - (M - 1) / 2 - 1];
          }
          left_block.size = (M - 1) / 2 * 2;
          for (int j = i; j < cur.size; ++j) {
            cur.child[j] = cur.child[j + 1];
          }
          for (int j = i - 1; j < cur.size - 1; ++j) {
            cur.data[j] = cur.data[j + 1];
          }
          --cur.size;
          index_file.write(left_block, left_block.ptr);
          if (cur.size < (M - 1) / 2) {
            return true;
          }
          index_file.write(cur,cur.ptr);
        }
      }
    }
    return false;
  }

  void print(int ptr) {
    IndexBlock index_block;
    index_file.read(index_block, ptr);
    if (index_block.flag) {
      std::cout << "ptr:" << index_block.ptr << ' ' << "flag:" << index_block.flag << ' ';
      std::cout << "keys:";
      for (int i = 0; i < index_block.size; ++i) {
        std::cout << index_block.data[i].value << ' ';
      }
      std::cout << "child:";
      for (int i = 0; i <= index_block.size; ++i) {
        std::cout << index_block.child[i] << ' ';
      }
      std::cout << '\n';
      for (int i = 0; i <= index_block.size; ++i) {
        print2(index_block.child[i]);
      }
      return;
    }
    std::cout << "ptr:" << index_block.ptr << ' ' << "flag:" << index_block.flag << ' ';
    std::cout << "keys:";
    for (int i = 0; i < index_block.size; ++i) {
      std::cout << index_block.data[i].value << ' ';
    }
    std::cout << "child:";
    for (int i = 0; i <= index_block.size; ++i) {
      std::cout << index_block.child[i] << ' ';
    }
    std::cout << '\n';
    for (int i = 0; i <= index_block.size; ++i) {
      print(index_block.child[i]);
    }
  }
  void print2(int ptr) {
    DataBlock data_block;
    data_file.read(data_block, ptr);
    std::cout << "ptr:" << data_block.ptr << ' ';
    std::cout << "data:";
    for (int i = 0; i < data_block.size; ++i) {
      std::cout << data_block.data[i].value << ' ';
    }
    std::cout << "nxt:" << data_block.nxt << '\n';
  }

public:
  BPT(const std::string& file1_name, const std::string& file2_name):
  index_file(file1_name), data_file(file2_name) {
    index_file.get_info(index_root, 1);
    index_file.get_info(size_of_index, 2);
    data_file.get_info(size_of_data, 2);
  };

  ~BPT() {
    index_file.write_info(index_root, 1);
    index_file.write_info(size_of_index, 2);
    data_file.write_info(size_of_data, 2);
  }

  void Print() {
    index_file.open();
    data_file.open();
    print(index_root);
    std::cout << '\n';
    index_file.close();
    data_file.close();
  }

  void Insert(long long index, int value) {
    index_file.open();
    data_file.open();
    pair pair_(index, value);
    if (!size_of_index) {
      IndexBlock index_block(size_of_index++);
      index_block.child[0] = 0;
      index_file.write(index_block, 0);
      DataBlock data_block(size_of_data++);
      data_block.data[0] = pair_;
      data_file.write(data_block, 0);
      index_file.close();
      data_file.close();
      return;
    }
    IndexBlock root;
    index_file.read(root, index_root);
    if (insert(root, pair_)) { // 根节点溢出了
      IndexBlock new_child(size_of_index++, root.flag, M - M / 2 - 1);
      for (int j = M / 2 + 1; j < M; ++j) {
        new_child.data[j - M / 2 - 1] = root.data[j];
      }
      for (int j = M / 2 + 1; j <= M; ++j) {
        new_child.child[j - M / 2 - 1] = root.child[j];
      }
      root.size = M / 2;
      IndexBlock new_root(size_of_index++, false, 1);
      new_root.child[0] = root.ptr;
      new_root.child[1] = new_child.ptr;
      new_root.data[0] = root.data[M / 2];
      index_root = new_root.ptr;
      index_file.write(root, root.ptr);
      index_file.write(new_root, new_root.ptr);
      index_file.write(new_child, new_child.ptr);
    }
    index_file.close();
    data_file.close();
  }

  void Delete(long long index, int value) {
    index_file.open();
    data_file.open();
    pair pair_(index, value);
    if (!size_of_index) {
      index_file.close();
      data_file.close();
      return;
    }
    IndexBlock root;
    index_file.read(root, index_root);
    if (Delete(root, pair_)) {
      if (!root.size && !root.flag) {
        index_root = root.child[0];
      }
      index_file.write(root, root.ptr);
    }
    index_file.close();
    data_file.close();
  }

  sjtu::vector<int> Find(long long index) {
    index_file.open();
    data_file.open();
    sjtu::vector<int> v;
    if (!size_of_index) { // 没有任何数据
      index_file.close();
      data_file.close();
      return v;
    }
    IndexBlock index_block;
    index_file.read(index_block, index_root);
    int i;
    while (!index_block.flag) { // tmp不是叶节点
      i = lower_bound(index_block.data, index_block.size, index); // index所在子节点的下标
      index_file.read(index_block, index_block.child[i]);
    }
    i = lower_bound(index_block.data, index_block.size, index); // index所在数据块的下标
    DataBlock data_block;
    data_file.read(data_block, index_block.child[i]);
    i = lower_bound(data_block.data, data_block.size, index); // index在数据块中的下标
    if (i == data_block.size) {
      data_file.read(data_block, data_block.nxt);
      i = 0;
    }
    while (data_block.data[i].index == index) {
      while (i < data_block.size && data_block.data[i].index == index) { // 数据块内循环
        v.push_back(data_block.data[i].value);
        ++i;
      }
      if (i == data_block.size && data_block.nxt >= 0) { // 继续找下一块
        data_file.read(data_block, data_block.nxt);
        i = 0;
      } else { // 找完了
        break;
      }
    }
    index_file.close();
    data_file.close();
    return v;
  }
};

#endif //BPT_HPP