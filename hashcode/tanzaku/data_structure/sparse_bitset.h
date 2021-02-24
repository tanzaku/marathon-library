#ifndef SPARSE_BITSET_H
#define SPARSE_BITSET_H

#include <bit>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

namespace tanzaku
{
namespace data_structure
{

using namespace std;
struct SparseBitset {
  vector<pair<int, uint64_t>> bits_;

  SparseBitset() {}
  SparseBitset(vector<pair<int, uint64_t>> &&bits)
      : bits_(bits)
  {
  }

  void insert(int i)
  {
    int index = i / 64;
    uint64_t bit = uint64_t(1) << (i % 64);

    size_t j = 0;
    while (j < bits_.size() && j < size_t(index))
      j++;

    if (j < bits_.size()) {
      if (bits_[j].first != index) {
        bits_.insert(bits_.begin() + j, make_pair(index, bit));
      } else {
        bits_[j].second |= bit;
      }
    } else {
      bits_.emplace_back(index, bit);
    }
  }

  void remove(int i)
  {
    int index = i / 64;
    uint64_t bit = uint64_t(1) << (i % 64);

    for (size_t j = 0;; j++) {
      if (bits_[j].first == index) {
        bits_[j].second &= ~bit;
        if (bits_[j].second == 0) {
          bits_.erase(bits_.begin() + j);
        }
        return;
      }
    }
  }

  SparseBitset unite(const SparseBitset &other) const
  {
    size_t i = 0;
    size_t j = 0;
    vector<pair<int, uint64_t>> bits(std::max(bits_.size(), other.bits_.size()));

    bits.clear();

    while (i < bits_.size() && j < other.bits_.size()) {
      if (bits_[i].first == other.bits_[j].first) {
        bits.emplace_back(bits_[i].first, bits_[i].second | other.bits_[j].second);
        i++;
        j++;
      } else if (bits_[i].first > other.bits_[j].first) {
        bits.push_back(other.bits_[j]);
        j++;
      } else {
        bits.push_back(bits_[i]);
        i++;
      }
    }

    while (i < bits_.size())
      bits.push_back(bits_[i++]);

    while (j < other.bits_.size())
      bits.push_back(other.bits_[j++]);

    return SparseBitset(std::move(bits));
  }

  void unite_with(const SparseBitset &other)
  {
    *this = unite(other);
  }

  SparseBitset intersect(const SparseBitset &other) const
  {
    size_t i = 0;
    size_t j = 0;
    vector<pair<int, uint64_t>> bits(std::max(bits_.size(), other.bits_.size()));

    bits.clear();

    while (i < bits_.size() && j < other.bits_.size()) {
      if (bits_[i].first == other.bits_[j].first) {
        uint64_t v = bits_[i].second & other.bits_[j].second;
        if (v != 0) {
          bits.emplace_back(bits_[i].first, v);
        }
        i++;
        j++;
      } else if (bits_[i].first > other.bits_[j].first) {
        j++;
      } else {
        i++;
      }
    }

    return SparseBitset(std::move(bits));
  }

  void intersect_with(const SparseBitset &other)
  {
    *this = intersect(other);
  }

  size_t count_ones() const
  {
    int ones = 0;
    for (const auto &bits : bits_) {
      ones += std::popcount(bits.second);
    }
    return ones;
  }
};

SparseBitset operator&(const SparseBitset &lhs, const SparseBitset &rhs)
{
  return lhs.intersect(rhs);
}

SparseBitset &operator&=(SparseBitset &lhs, const SparseBitset &rhs)
{
  lhs.intersect_with(rhs);
  return lhs;
}

SparseBitset operator|(const SparseBitset &lhs, const SparseBitset &rhs)
{
  return lhs.unite(rhs);
}

SparseBitset &operator|=(SparseBitset &lhs, const SparseBitset &rhs)
{
  lhs.unite_with(rhs);
  return lhs;
}

} // namespace data_structure
} // namespace tanzaku

#endif
