
#include <vector>

using namespace std;

/**
 * O(1)でクリアできるマップ
 * キーが[0,n)の整数でなくてはならない
 */
template <typename T>
class CookieMap
{
  int gid;
  std::vector<int> id;
  std::vector<T> val;
  T defaultValue;

public:
  CookieMap(int n, T defaultValue_) : gid(1), id(n), val(n), defaultValue(defaultValue_) {}
  bool contains(int i) const { return id[i] == gid; }
  void clear() { gid++; }
  void set(int i, T v)
  {
    id[i] = gid;
    val[i] = v;
  }
  T get(int i) const
  {
    return contains(i) ? val[i] : defaultValue;
  }
  T &operator[](int i)
  {
    if (!contains(i)) {
      id[i] = gid;
      val[i] = defaultValue;
    }
    return val[i];
  }
};
