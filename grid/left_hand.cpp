#include "grid.h"
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

struct LeftHandTour {
  Grid grid;
  Cursor c;

  void step()
  {
    rotL();
    while (!grid.can_move(c.p, c.dir)) {
      rotR();
    }
    grid.move(c);
  }

  bool can_move() const
  {
  }

  int left_dir() const
  {
    return (c.dir + 1) & 3;
  }

  void rotL()
  {
    c.dir = (c.dir + 1) & 3;
  }

  void rotR()
  {
    c.dir = (c.dir + 3) & 3;
  }
};

// struct GridTourLeftHand {
//   int tryDir;
//   int forwardDir;
//   string moves;
//   vector<vector<uint8_t>> board;
//   stack<tuple<int, int, int>> history;
//   int x, y;

//   void init()
//   {
//     forwardDir = 0;
//     x = y = 0;
//     tryDir = 1;
//     visited[y][x]++;
//     board.assign(N, vector<uint8_t>(N));
//     board[y][x] = snake[0];
//   }

//   int index() const
//   {
//     return isGoal() ? 1 << 29 : (int)moves.size();
//   }

//   void reverse()
//   {
//     tryDir = 4 - tryDir;
//   }

//   bool isVisited(int xx, int yy)
//   {
//     return !isValid(xx, yy) || visited[yy][xx];
//   }

//   bool canMove(int dir) const
//   {
//     int xx = x, yy = y;
//     ::step(xx, yy, MOVE_DIRS[dir]);
//     if (!isValid(xx, yy) || visited[yy][xx]) {
//       // cerr << xx << " " << yy << " is NG" << endl;
//       return false;
//     }
//     return isConstructable(xx, yy);
//   }

//   void addDir(int &dir, int v) const
//   {
//     dir = (dir + v) & 3;
//   }

//   bool isGoal() const
//   {
//     return ::isGoal(x, y);
//   }

//   int getMoveDir() const
//   {
//     if (isGoal()) {
//       return -1;
//     }
//     int tmpDir = forwardDir;
//     addDir(tmpDir, tryDir);
//     for (;; addDir(tmpDir, 4 - tryDir)) {
//       if (canMove(tmpDir)) {
//         return tmpDir;
//       }
//     }
//   }

//   void step()
//   {
//     if (!isGoal()) {
//       stepByDir(getMoveDir());
//     }
//   }

//   bool stepByDir(int dir)
//   {
//     if (isGoal()) {
//       return false;
//     }
//     if (!canMove(dir)) {
//       return false;
//     }

//     history.emplace(x, y, forwardDir);
//     forwardDir = dir;
//     ::step(x, y, MOVE_DIRS[forwardDir]);
//     moves += MOVE_DIRS[forwardDir];
//     visited[y][x]++;
//     board[y][x] = snake[moves.length()];
//     return true;
//   }

//   void unstep()
//   {
//     visited[y][x]--;
//     tie(x, y, forwardDir) = history.top();
//     history.pop();
//     moves.pop_back();
//   }

//   int distToAdj(int tx, int ty) const
//   {
//     if (isGoal()) {
//       return 1 << 29;
//     }

//     int xx = x, yy = y, d = 0;
//     vector<tuple<int, int, int>> que;
//     que.emplace_back(xx, yy, d);
//     int res = 1 << 29;
//     for (int i = 0; i < (int)que.size(); i++) {
//       tie(xx, yy, d) = que[i];
//       if (std::abs(tx - xx) + std::abs(ty - yy) == 1) {
//         res = d;
//         break;
//       }
//       for (char dir : MOVE_DIRS) {
//         tie(xx, yy, d) = que[i];
//         ::step(xx, yy, dir);
//         if (!isValid(xx, yy) || visited[yy][xx]) continue;
//         visited[yy][xx]++;
//         que.emplace_back(xx, yy, d + 1);
//       }
//     }
//     for (int i = 1; i < (int)que.size(); i++) {
//       tie(xx, yy, d) = que[i];
//       visited[yy][xx]--;
//     }
//     return res;
//   }

//   tuple<int, int> curPos() const
//   {
//     return make_tuple(x, y);
//   }
// };
