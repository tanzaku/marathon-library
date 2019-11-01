template<int w, int h>
struct BIT2D {
  int bit[(w+1)][(h+1)];

  void clear() { memset(bit, 0, sizeof(bit)); }

  int sum(int x, int y_) {
    int res = 0;
    for (; x > 0; x -= x & -x)
      for (int y = y_; y > 0; y -= y & -y)
	res += bit[x][y];
    return res;
  }

  // 1<=i<=n
  void add(int x, int y_, int v) {
    for (; x <= w; x += x & -x)
      for (int y = y_; y <= h; y += y & -y)
	bit[x][y] += v;
  }
};
