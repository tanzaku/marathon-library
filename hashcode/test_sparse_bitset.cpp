#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <utility>

#include "sparse_bitset.h"

#include <set>

template <typename T>
void assert_eq(T lhs, T rhs)
{
  if (lhs != rhs) {
    throw;
  }
}

// using namespace tanzaku::data_structure;

void bitor_check(tanzaku::data_structure::SparseBitset &lhs, tanzaku::data_structure::SparseBitset &rhs, size_t expected)
{
  auto b = lhs;
  b |= rhs;
  assert_eq(b.count_ones(), expected);
};

void bitand_check(tanzaku::data_structure::SparseBitset &lhs, tanzaku::data_structure::SparseBitset &rhs, size_t expected)
{
  auto b = lhs;
  b &= rhs;
  assert_eq(b.count_ones(), expected);
};

int main()
{
  std::set<int> st;
  tanzaku::data_structure::SparseBitset b1;
  tanzaku::data_structure::SparseBitset b2;
  tanzaku::data_structure::SparseBitset b3;

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;
  b1.insert(0);

  b2.insert(0);
  b2.insert(65);
  b2.insert(64 * 3 - 1);

  b3.insert(64 * 2);
  b3.insert(64 * 2 + 1);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  assert_eq<size_t>(b1.count_ones(), 1);
  assert_eq<size_t>(b2.count_ones(), 3);
  assert_eq<size_t>(b3.count_ones(), 2);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  assert_eq<size_t>((b1 | b2).count_ones(), 3);
  assert_eq<size_t>((b1 | b3).count_ones(), 3);
  assert_eq<size_t>((b2 | b3).count_ones(), 5);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  assert_eq<size_t>((b1 & b2).count_ones(), 1);
  assert_eq<size_t>((b1 & b3).count_ones(), 0);
  assert_eq<size_t>((b2 & b3).count_ones(), 0);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  bitor_check(b2, b1, 3);
  bitor_check(b3, b1, 3);
  bitor_check(b3, b2, 5);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  bitand_check(b2, b1, 1);
  bitand_check(b3, b1, 0);
  bitand_check(b3, b2, 0);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  b2.remove(0);
  assert_eq<size_t>(b2.count_ones(), 2);

  std::cerr << __FUNCTION__ << " " << __LINE__ << std::endl;

  std::cerr << "OK" << std::endl;
}
