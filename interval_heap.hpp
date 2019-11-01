

#include <functional>
#include <vector>

template <typename T, class Compare = std::less<T>>
class interval_heap {
  std::vector<T> data;
  Compare comp;

  void up_min(size_t i);
  void up_max(size_t i);
  void down_min();
  void down_max();

 public:
  interval_heap(Compare c = Compare()) : comp(c) {}
  interval_heap(size_t capacity, Compare c = Compare())
      : data(capacity), comp(c) {
    data.clear();
  }

  void push(T val);
  T peek_min() const;
  T peek_max() const;
  T pop_min();
  T pop_max();

  bool empty() const;
  size_t size() const;
};

template <typename T, class Compare>
interval_heap<T, Compare> make_interval_heap(Compare cmp) {
  return interval_heap<T, Compare>(cmp);
}

template <typename T, class Compare>
void interval_heap<T, Compare>::push(T val) {
  size_t i = data.size();
  data.push_back(val);
  if ((i & 1) && !comp(data[i - 1], data[i])) {
    std::swap(data[i - 1], data[i]);
    i--;
  }
  up_min(i);
  up_max(i);
}

template <typename T, class Compare>
void interval_heap<T, Compare>::up_min(size_t i) {
  for (size_t j; i > 1; i = j) {
    j = (i / 2 - 1) & ~1;
    if (comp(data[j], data[i])) break;
    std::swap(data[j], data[i]);
  }
}

template <typename T, class Compare>
void interval_heap<T, Compare>::up_max(size_t i) {
  for (size_t j; i > 1; i = j) {
    j = (i / 2 - 1) | 1;
    if (!comp(data[j], data[i])) break;
    std::swap(data[j], data[i]);
  }
}

template <typename T, class Compare>
void interval_heap<T, Compare>::down_min() {
  for (size_t i = 0, j;; i = j) {
    const size_t l = 2 * i + 2;
    if (l >= data.size()) {
      if (i + 1 < data.size() && !comp(data[i], data[i + 1])) {
        std::swap(data[i], data[i + 1]);
        i++;
      }
      up_max(i);
      break;
    }

    const size_t r = 2 * i + 4;
    j = r >= data.size() || comp(data[l], data[r]) ? l : r;
    if (!comp(data[j], data[i])) {
      break;
    }
    std::swap(data[i], data[j]);
  }
}

template <typename T, class Compare>
void interval_heap<T, Compare>::down_max() {
  for (size_t i = 1, j;; i = j) {
    const size_t l = 2 * i + 1;
    if (l >= data.size()) {
      if (i > 0 < data.size() && comp(data[i], data[i - 1])) {
        std::swap(data[i], data[i - 1]);
        i--;
      }
      up_min(i);
      break;
    }

    const size_t r = 2 * i + 3;
    j = r >= data.size() || !comp(data[l], data[r]) ? l : r;
    if (comp(data[j], data[i])) {
      break;
    }
    std::swap(data[i], data[j]);
  }
}

template <typename T, class Compare>
T interval_heap<T, Compare>::pop_min() {
  auto res = data[0];
  if (data.size() > 1) {
    data[0] = data.back();
  }
  data.pop_back();
  down_min();
  return res;
}

template <typename T, class Compare>
T interval_heap<T, Compare>::pop_max() {
  if (data.size() == 1) {
    return pop_min();
  }

  auto res = data[1];
  if (data.size() > 2) {
    data[1] = data.back();
  }
  data.pop_back();
  down_max();
  return res;
}

template <typename T, class Compare>
T interval_heap<T, Compare>::peek_min() const {
  return data[0];
}

template <typename T, class Compare>
T interval_heap<T, Compare>::peek_max() const {
  return data.size() == 1 ? data[0] : data[1];
}

template <typename T, class Compare>
bool interval_heap<T, Compare>::empty() const {
  return size() == 0;
}

template <typename T, class Compare>
size_t interval_heap<T, Compare>::size() const {
  return data.size();
}
