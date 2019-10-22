#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * [0,n)の集合を扱うデータ構造
 * 操作をO(1)で実行できる
 */
class SeqSet
{
  struct OpHistory {
    int i, j;
    int d;

    OpHistory() {}
    OpHistory(int i, int j, int d) : i(i), j(j), d(d) {}
  };

  struct Iterator {
    vector<int>::iterator i;

    Iterator() {}
    Iterator(vector<int>::iterator i) : i(i) {}

    bool operator!=(Iterator it) const
    {
      return i != it.i;
    }

    void operator++()
    {
      i++;
    }

    int operator*()
    {
      return *i;
    }
  };

  int capacity;
  int len;
  vector<int> value;
  vector<int> index;
  vector<OpHistory> history;

  void swapValue(int i, int j)
  {
    auto &x = value[i];
    auto &y = value[j];
    std::swap(x, y);
    std::swap(index[x], index[y]);
  }

public:
  void init(int c)
  {
    capacity = c;
    len = 0;
    value.clear();
    value.reserve(capacity);
    index.clear();
    index.reserve(capacity);
    for (int i = 0; i < capacity; i++) {
      value.push_back(i);
      index.push_back(i);
    }
  }

  Iterator begin()
  {
    return Iterator(value.begin());
  }

  Iterator end()
  {
    return Iterator(value.begin() + len);
  }

  void commit()
  {
    history.clear();
  }

  void rollback()
  {
    while (!history.empty()) {
      auto h = history.back();
      history.pop_back();
      swapValue(h.i, h.j);
      len -= h.d;
    }
  }

  bool contains(int v) const
  {
    return index[v] < len;
  }

  int size() const
  {
    return len;
  }

  int unusedSize() const
  {
    return capacity - len;
  }

  void removeByValue(int v)
  {
    removeByIndex(index[v]);
  }

  void removeByIndex(int i)
  {
    if (i < 0 || i >= len) {
      cerr << "Error on SeqSet#remove: " << i << " " << len << endl;
      throw;
    }

    len--;
    swapValue(i, len);
    history.emplace_back(i, len, -1);
  }

  void insertByValue(int v)
  {
    insertByIndex(index[v]);
  }

  /**
   * i : [0, unusedSize)
   */
  void insertByIndex(int i)
  {
    if (i < 0 || i >= capacity) {
      cerr << "Error on SeqSet#insert: " << i << " " << capacity << endl;
      throw;
    }

    swapValue(i, len);
    history.emplace_back(i, len, 1);
    len++;
  }

  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity; }
  void pop_back() { removeByIndex(len - 1); }
  void push_back(int v) { insertByValue(v); }

  int operator[](int i) const { return value[i]; }

  void verify()
  {
    for (int v = 0; v < capacity; v++) {
      if (value[index[v]] != v) {
        cerr << "verify failed : " << v << " " << index[v] << " " << value[index[v]] << endl;
        throw;
      }
    }
  }
};
