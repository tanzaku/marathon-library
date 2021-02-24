#ifndef FAST_SET_H
#define FAST_SET_H

#include <numeric>
#include <vector>

namespace tanzaku
{
namespace data_structure
{

using namespace std;
struct FastSet {
  struct Slice {
    const int *ptr_;
    int len_;

    Slice(const int *ptr, int len) : ptr_(ptr), len_(len) {}

    int operator[](int i) const { return ptr_[i]; }
    int len() const { return len_; }
  };

  vector<int> indices_;
  vector<int> values_;
  int len_;
  int capacity_;

  FastSet(int capacity)
  {
    indices_.resize(capacity);
    values_.resize(capacity);

    iota(indices_.begin(), indices_.end(), 0);
    iota(values_.begin(), values_.end(), 0);
    len_ = 0;
    capacity_ = capacity;
  }

  void insert(int value)
  {
    if (contains(value)) {
      throw;
    }

    int i = indices_[value];
    if (i != len_) {
      int v = values_[len_];
      swap(indices_[value], indices_[v]);
      swap(values_[i], values_[len_]);
    }
    len_ += 1;
  }

  void remove(int value)
  {
    if (!contains(value)) {
      throw;
    }

    len_ -= 1;
    int i = indices_[value];
    if (i != len_) {
      int v = values_[len_];
      swap(indices_[value], indices_[v]);
      swap(values_[i], values_[len_]);
    }
  }

  bool contains(int value)
  {
    return indices_[value] < len_;
  }

  Slice set() const
  {
    return Slice(values_.data(), len_);
  }

  Slice complementary_set() const
  {
    return Slice(values_.data() + len_, capacity_ - len_);
  }

  int len() const
  {
    return len_;
  }

  int capacity() const
  {
    return capacity_;
  }

  bool is_full() const
  {
    return len_ == capacity_;
  }

  bool is_empty() const
  {
    return len_ == 0;
  }
};

} // namespace data_structure
} // namespace tanzaku

#endif
