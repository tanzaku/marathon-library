#include <vector>

/**
 * O(1)でクリアできるセット
 * キーが[0,n)の整数でなくてはならない
 */
class Cookie
{
  int gid;
  std::vector<int> id;

public:
  Cookie(int cookieSize) : gid(0), id(cookieSize, 0) {}
  bool contains(int i) const { return id[i] == gid; }
  void clear() { gid++; }
  void set(int i)
  {
    id[i] = gid;
  }
  void remove(int i)
  {
    id[i] = gid - 1;
  }
};
