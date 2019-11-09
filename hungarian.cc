#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <queue>
#include <tuple>
#include <vector>

using namespace std;

#include <iostream>

using W = int64_t;
vector<tuple<int, int>> hungarian(vector<vector<W>> weights)
{
  const int n = (int)weights.size();
  const int m = (int)weights[0].size();
  const int s = std::min(n, m);

  vector<tuple<int, int>> assignment;
  assignment.reserve(s);

    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) {
        cerr << weights[y][x] << " ";
      }
      cerr << endl;
    }

    for (int y = 0; y < n; y++) {
      W lowest = std::numeric_limits<W>::max();
      for (int x = 0; x < m; x++)
        lowest = std::min(lowest, weights[y][x]);
      for (int x = 0; x < m; x++)
        weights[y][x] -= lowest;
    }

    for (int x = 0; x < m; x++) {
      W lowest = std::numeric_limits<W>::max();
      for (int y = 0; y < n; y++)
        lowest = std::min(lowest, weights[y][x]);
      for (int y = 0; y < n; y++)
        weights[y][x] -= lowest;
    }

  while (!rowIdx.empty()) {
    vector<bool> markedX(m, false);
    vector<bool> markedY(n, false);
    assignment.clear();
    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) {
        if (!markedX[x] && weights[y][x] == 0) {
          marked[x] = true;
          marked[y] = true;
          assignment.emplace_back(y, x);
          break;
        }
      }
    }

    if (assignment.size() == s) {
      break;
    }

    W v = numeric_limits<W>::max();
    for (int y = 0; y < n; y++) if (!markedY[y]) {
      for (int x = 0; x < m; x++) if (!markedX[x]) {
        v = std::min(v, weights[y][x]);
      }
    }

    for (int y = 0; y < n; y++) {
      for (int x = 0; x < m; x++) if () {
        if (!markedY[y] && !markedX[x]) { weights[y][x] -= v; }
        if (markedY[y] && markedX[x]) { weights[y][x] += v; }
      }
    }
  }
  return assignment;
}

int main()
{
  int n;
  cin >> n;

  vector<vector<int64_t>> w(n, vector<int64_t>(n));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      int v;
      cin >> v;
      w[i][j] = v;
    }
  }

  int64_t ans = 0;
  auto ps = hungarian(w);
  sort(ps.begin(), ps.end());
  for (auto p : ps) {
    ans += w[get<0>(p)][get<1>(p)];
  }
  cout << ans << endl;
  for (int i = 0; i < ps.size(); i++) {
    if (i) cout << " ";
    cout << get<1>(ps[i]);
  }
  cout << endl;
  return 0;
}

// int main()
// {
//   int n, m;
//   cin >> n >> m;

//   string s;
//   int mj, x, y;
//   vector<tuple<int, int, int>> a, b;
//   for (int i = 0; i < n; i++) {
//     cin >> s >> x >> y;
//     a.emplace_back(x, y, 0);
//   }
//   for (int i = 0; i < m; i++) {
//     cin >> s >> mj >> x >> y;
//     b.emplace_back(x, y, mj);
//   }
//   int sz = std::max(n, m);
//   vector<vector<int64_t>> w(sz, vector<int64_t>(sz, 1LL << 60));

//   for (int i = 0; i < n; i++) {
//     for (int j = 0; j < m; j++) {
//       int x0, y0, x1, y1;
//       tie(x0, y0, mj) = a[i];
//       tie(x1, y1, mj) = b[j];
//       int64_t mm = ((1LL << 20) - mj) << 32;
//       // cerr << i << " " << j << " " << mm << endl;
//       w[i][j] = mm | (abs(x0 - x1) + abs(y0 - y1));
//     }
//   }
//   int ans = 0;
//   auto ps = hungarian(w);
//   for (auto p : ps) {
//     ans += w[get<0>(p)][get<1>(p)] & 0xFFFFFFFFLL;
//   }
//   cout << ans << endl;
//   return 0;
// }
