// union find
#include <algorithm>
#include <array>
#include <queue>
#include <tuple>
#include <vector>

using namespace std;

struct union_find {
  vector<int> data;

  void init(int size)
  {
    data.resize(size);
    clear();
  }

  void clear()
  {
    fill(data.begin(), data.end(), -1);
  }

  int root(int x) { return data[x] < 0 ? x : (data[x] = root(data[x])); }

  void unite(int x, int y)
  {
    if ((x = root(x)) != (y = root(y))) {
      if (data[y] < data[x]) {
        swap(x, y);
      }
      data[x] += data[y];
      data[y] = x;
    }
  }

  bool same(int x, int y) { return root(x) == root(y); }
  int size(int x) { return -data[root(x)]; }
};
