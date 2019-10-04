

#include <tuple>
#include <vector>

/**
 * 整数かつ優先度が小さい場合のみ高速に動作するpriority queue
 * 高速化のためpopしても要素は解放されないので注意
 * clear時は解放される
 * 
 * 優先度の値が小さい方が優先度が高い=先に出てくる
 */
template <typename T>
class PriorityQueueWithLittleIntPriority
{
  int size_ = 0;
  int cursor;
  std::vector<int> queIdx;
  std::vector<std::vector<std::tuple<int, T>>> que;

  void moveCursorNext()
  {
    if (empty()) {
      throw;
    }
    for (cursor = 0;; cursor++) {
      if ((int)que[cursor].size() > queIdx[cursor])
        return;
    }
  }

public:
  PriorityQueueWithLittleIntPriority(int maxPriority)
  {
    que.resize(maxPriority + 1);
    queIdx.resize(maxPriority + 1);
    clear();
  }

  void clear()
  {
    for (auto &q : que)
      q.clear();
    for (auto &q : queIdx)
      q = 0;
    size_ = 0;
  }

  void emplace(int priority, T val)
  {
    if ((int)que.size() <= priority) {
      que.resize(priority + 1);
      queIdx.resize(priority + 1);
    }
    que[priority].emplace_back(priority, val);
    size_++;
  }

  std::tuple<int, T> &top()
  {
    moveCursorNext();
    return que[cursor][queIdx[cursor]];
  }

  /**
   * top使用後に使う前提
   */
  void pop()
  {
    queIdx[cursor]++;
    size_--;
  }

  bool empty() const
  {
    return size_ == 0;
  }

  int size() const
  {
    return size_;
  }
};
