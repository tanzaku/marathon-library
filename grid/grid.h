
#include <algorithm>
#include <array>
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

using CELL = int;

struct Cursor {
  int p, dir;
};

struct Grid {
  int w, h;

  array<int, 4> pos_d;
  vector<array<int, 4>> can_move_dir;
  vector<vector<int>> adjacent;
  vector<CELL> cell;

  void init(int w_, int h_)
  {
    this->w = w_;
    this->h = h_;

    // RULD
    pos_d[0] = 1;
    pos_d[1] = -w;
    pos_d[2] = -1;
    pos_d[3] = w;
    can_move.resize(w * h);
    adjacent.resize(w * h);

    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        for (int i = 0; i < 4; i++) {
          int xx = x + pos_d[i] % w;
          int yy = y + pos_d[i] / w;
          can_move[enc(x, y)][i] = is_valid(xx, yy);
          if (can_move[enc(x, y)][i]) {
            adjacent[enc(x, y)].push_back(enc(xx, yy));
          }
        }
      }
    }
  }

  uint enc(uint x, uint y) const
  {
    return y * w + x;
  }

  tuple<uint, uint> dec(uint p) const
  {
    return make_tuple(p % w, p / w);
  }

  bool is_valid(int x, int y)
  {
    return 0 <= x && x < w && 0 <= y && y < h;
  }

  bool can_move(int p, int dir) const
  {
    return can_move_dir[p][dir];
  }

  void move(Cursor c)
  {
    c.p += pos_d[c.dir];
  };
};
