#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <utility>

#include "fast_set.h"

#include <set>

int main()
{
  std::set<int> st;
  tanzaku::data_structure::FastSet fast_set(100);

  int v = 0;
  for (int i = 0; i < 100000000; i++) {
    v = (v * 38 + 7) % 100;
    bool c1 = !!st.count(v);
    bool c2 = fast_set.contains(v);

    // std::cerr << v << " " << c1 << " " << c2 << std::endl;

    if (c1 != c2) {
      std::cerr << __FUNCTION__ << " " << __LINE__ << " "
                << "NG" << std::endl;
      throw;
    }

    auto s = fast_set.set();
    if (s.len() != int(st.size())) {
      std::cerr << __FUNCTION__ << " " << __LINE__ << " "
                << "NG" << std::endl;
      throw;
    }

    auto cs = fast_set.complementary_set();
    if (cs.len() != 100 - int(st.size())) {
      std::cerr << __FUNCTION__ << " " << __LINE__ << " "
                << "NG" << std::endl;
      throw;
    }

    if (c2) {
      st.erase(v);
      fast_set.remove(v);
    } else {
      st.insert(v);
      fast_set.insert(v);
    }
  }

  std::cerr << "OK" << std::endl;
}
