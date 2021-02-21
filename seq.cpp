#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


class Seq
{
  struct OpHistory {
    int i, j;

    OpHistory() {}
    OpHistory(int i, int j) : i(i), j(j) {}
  };

  vector<int> seq;
  vector<OpHistory> history;

public:
  void push(int v)
  {
    seq.push_back(v);
  }

  vector<int>::iterator begin()
  {
    return seq.begin();
  }

  vector<int>::iterator end()
  {
    return seq.end();
  }

  int size() const
  {
    return seq.size();
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
      std::swap(seq[h.i], seq[h.j]);
    }
  }

  void swap(int i, int j)
  {
    std::swap(seq[i], seq[j]);
    history.emplace_back(i, j);
  }

  int operator[](int i) const { return seq[i]; }
};
