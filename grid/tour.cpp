// C++11
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

// constexprでコンパイル時に列挙
// 最大で7x7前提
constexpr int ROW_SIZE = 8;
struct GridTour {
  uint w, h;

  vector<vector<uint>> tours;
  vector<uint> cur_tours;
  vector<uint> shrinked_pos;
  vector<vector<uint>> adj;

  GridTour &init(uint w_, uint h_)
  {
    this->w = w_;
    this->h = h_;

    visited = ~uint64_t(0);

    for (int i = 0; i < int(h); i++) {
      visited &= ~(((uint64_t(1) << w) - 1) << (i * ROW_SIZE));
    }
    // cerr << visited << " " << (visited | 7 | 7 << 8 | 7 << 16) << " " << (1 + (visited | 7 | 7 << 8 | 7 << 16)) << endl;

    adj.resize(ROW_SIZE * h);
    shrinked_pos.resize(ROW_SIZE * h);
    int d[] = {1, 0, -1, 0, 1};
    for (int y = 0; y < int(h); y++) {
      for (int x = 0; x < int(w); x++) {
        for (int i = 0; i < 4; i++) {
          int tx = x + d[i], ty = y + d[i + 1];
          if (is_valid(tx, ty)) {
            adj[y * ROW_SIZE + x].push_back(ty * ROW_SIZE + tx);
            shrinked_pos[y * ROW_SIZE + x] = y * w + x;
          }
        }
      }
    }

    // 必ず左上始点
    enumerate_tour(0);
    // enumerate_tour_nonrecursive(0);
    return *this;
  }

  uint enc(uint x, uint y) const
  {
    return y * w + x;
  }

  tuple<uint, uint> dec(uint p) const
  {
    return make_tuple(p % w, p / w);
  }

private:
  uint64_t visited;
  bool coverable(int s)
  {
    uint64_t coverable_bit = uint64_t(1) << s;
    while (1) {
      uint64_t next_coverable_bit = (coverable_bit | coverable_bit << ROW_SIZE | coverable_bit >> ROW_SIZE | coverable_bit << 1 | coverable_bit >> 1) & ~visited;

      if ((next_coverable_bit | visited) == numeric_limits<uint64_t>::max()) {
        return true;
      }

      if (next_coverable_bit == coverable_bit) {
        return false;
      }
      coverable_bit = next_coverable_bit;
    }
  }

  bool is_valid(int x, int y)
  {
    return 0 <= x && x < int(w) && 0 <= y && y < int(h);
  }

  void enumerate_tour(uint p)
  {
    if (!coverable(p)) {
      // if ((visited >> p & 1) || !coverable(p)) {
      // if (reachable_goal(p)) { // 数え上げお姉さん
      return;
    }

    visited |= uint64_t(1) << p;
    cur_tours.push_back(shrinked_pos[p]);

    if (visited == numeric_limits<uint64_t>::max()) {
      // if (p == (h - 1) * ROW_SIZE + w - 1 && visited == numeric_limits<uint64_t>::max()) { // 終点固定
      tours.push_back(cur_tours);
      // if (p == (h - 1) * ROW_SIZE + w - 1) { // 終点
      //   print();
      // }
    }

    for (uint a : adj[p]) {
      if (!(visited >> a & 1)) {
        enumerate_tour(a);
      }
    }

    cur_tours.pop_back();
    visited &= ~(uint64_t(1) << p);
  }

  // void enumerate_tour_nonrecursive(int p)
  // {
  //   stack<int> st;
  //   st.push(p);
  //   while (!st.empty()) {
  //     p = st.top();
  //     st.pop();

  //     if (p < 0) {
  //       cur_tours.pop_back();
  //       visited &= ~(uint64_t(1) << (-p - 1));
  //       continue;
  //     }

  //     // cerr << p << endl;
  //     cur_tours.push_back(shrinked_pos[p]);
  //     if (!coverable(p)) {
  //       // if ((visited >> p & 1)) { // 数え上げお姉さん
  //       continue;
  //     }
  //     visited |= uint64_t(1) << p;

  //     if (visited == numeric_limits<uint64_t>::max()) {
  //       // if (p == (h - 1) * ROW_SIZE + w - 1 && visited == numeric_limits<uint64_t>::max()) { // 終点固定
  //       tours.push_back(cur_tours);
  //     }

  //     for (int a : adj[p]) {
  //       if (!(visited >> a & 1)) {
  //         st.push(-a - 1);
  //         st.push(a);
  //       }
  //     }
  //   }
  // }

  // void print() const
  // {
  //   for (uint p : cur_tours) {
  //     // cerr << " -> (" << (p % ROW_SIZE) << "," << (p / ROW_SIZE) << ")";
  //     cerr << " -> (" << (p % w) << "," << (p / w) << ")";
  //   }
  //   cerr << endl;
  // }
};

int main()
{
  for (int i = 7; i <= 7; i++) {
    cerr << i << " " << GridTour().init(i, i).tours.size() << endl;
  }
  return 0;
}
