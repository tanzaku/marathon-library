
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

using W = float;

struct Shape {
  int batch, channel, rows, cols;

  int size() const
  {
    return batch * channel * rows * cols;
  }

  int operator()(int b, int c, int y, int x) const
  {
    b %= batch; // broadcatsing for batch
    // c %= channel;
    // y %= rows;
    // x %= cols;
    assert(c < channel);
    assert(y < rows);
    assert(x < cols);
    return ((b * channel + c) * rows + y) * cols + x;
  }
};

Shape operator+(const Shape &lhs, const Shape &rhs)
{
  assert(lhs.batch % rhs.batch == 0);
  assert(lhs.channel == rhs.channel);
  assert(lhs.rows == rhs.rows);
  assert(lhs.cols == rhs.cols);
  return Shape{lhs.batch, rhs.channel, lhs.rows, rhs.cols};
}

Shape operator*(const Shape &lhs, const Shape &rhs)
{
  assert(lhs.batch % rhs.batch == 0);
  assert(lhs.cols == rhs.rows);
  return Shape{lhs.batch, rhs.channel, lhs.rows, rhs.cols};
}

Shape transpose(const Shape &s)
{
  assert(s.batch == 1);
  return Shape{s.batch, s.channel, s.cols, s.rows};
}

Shape fold_by_axis(const Shape &s, int axis)
{
  if (axis == 0) {
    return Shape{1, s.channel, s.rows, s.cols};
  }
  if (axis == 1) {
    return Shape{s.batch, 1, s.rows, s.cols};
  }
  if (axis == 2) {
    return Shape{s.batch, s.channel, 1, s.cols};
  }
  if (axis == 3) {
    return Shape{s.batch, s.channel, s.rows, 1};
  }
  throw;
}

/**
 * 
 */
struct Tensor {
  Shape shape;
  std::vector<W> data;

  Tensor() {}
  Tensor(Shape shape_) : shape(shape_), data(shape.size(), 0) {}
  Tensor(std::initializer_list<W> lst1) : shape({1, 1, 1, static_cast<int>(lst1.size())}), data(lst1) {}
  Tensor(std::initializer_list<initializer_list<W>> lst2) : shape({1, 1, static_cast<int>(lst2.size()), static_cast<int>(std::begin(lst2)->size())})
  {
    data.reserve(shape.size());
    for (auto &lst1 : lst2) {
      std::copy(std::begin(lst1), std::end(lst1), std::back_inserter(data));
    }
  }

  W &operator()(int b, int c, int y, int x)
  {
    return data[shape(b, c, y, x)];
  }

  W operator()(int b, int c, int y, int x) const
  {
    return data[shape(b, c, y, x)];
  }
};

void dump(std::string name, const Tensor &t)
{
  auto shape = t.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          std::cerr << name << ": " << b << " " << c << " " << y << " " << x << " " << t(b, c, y, x) << std::endl;
        }
      }
    }
  }
}

// Tensor operator+(const Tensor &lhs, const Tensor &rhs)
// {
//   auto newShape = lhs.shape + rhs.shape;
//   Tensor t(newShape);
//   for (int b = 0; b < newShape.batch; b++) {
//     for (int c = 0; c < newShape.channel; c++) {
//       for (int y = 0; y < newShape.rows; y++) {
//         for (int x = 0; x < newShape.cols; x++) {
//           t(b, c, y, x) = lhs(b, c, y, x) * rhs(b, c, y, x);
//         }
//       }
//     }
//   }
//   return t;
// }

Tensor dot(const Tensor &lhs, const Tensor &rhs)
{
  auto newShape = lhs.shape * rhs.shape;
  Tensor t(newShape);
  for (int b = 0; b < newShape.batch; b++) {
    for (int co = 0; co < newShape.channel; co++) {
      for (int ci = 0; ci < lhs.shape.channel; ci++) {
        for (int y = 0; y < newShape.rows; y++) {
          for (int k = 0; k < lhs.shape.cols; k++) {
            for (int x = 0; x < newShape.cols; x++) {
              t(b, co, y, x) += lhs(b, ci, y, k) * rhs(b, co, k, x);
            }
          }
        }
      }
    }
  }
  return t;
}

Tensor transpose(const Tensor &v)
{
  auto newShape = transpose(v.shape);
  Tensor t(newShape);
  for (int b = 0; b < newShape.batch; b++) {
    for (int c = 0; c < newShape.channel; c++) {
      for (int y = 0; y < newShape.rows; y++) {
        for (int x = 0; x < newShape.cols; x++) {
          t(b, c, y, x) = v(b, c, x, y);
        }
      }
    }
  }
  return t;
}

Tensor &operator+=(Tensor &lhs, const Tensor &rhs)
{
  auto shape = lhs.shape + rhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) += rhs(b, c, y, x);
        }
      }
    }
  }
  return lhs;
}

Tensor &operator-=(Tensor &lhs, const Tensor &rhs)
{
  auto shape = lhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) -= rhs(b, c, y, x);
        }
      }
    }
  }
  return lhs;
}

Tensor &operator*=(Tensor &lhs, const Tensor &rhs)
{
  auto shape = lhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) *= rhs(b, c, y, x);
        }
      }
    }
  }
  return lhs;
}

Tensor &operator*=(Tensor &lhs, W rhs)
{
  auto shape = lhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) *= rhs;
        }
      }
    }
  }
  return lhs;
}

Tensor &operator*=(W lhs, Tensor &rhs)
{
  auto shape = rhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          rhs(b, c, y, x) *= lhs;
        }
      }
    }
  }
  return rhs;
}

Tensor &operator/=(Tensor &lhs, const Tensor &rhs)
{
  auto shape = lhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) /= rhs(b, c, y, x);
        }
      }
    }
  }
  return lhs;
}

Tensor &operator/=(Tensor &lhs, W rhs)
{
  auto shape = lhs.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          lhs(b, c, y, x) /= rhs;
        }
      }
    }
  }
  return lhs;
}

Tensor operator+(const Tensor &lhs, const Tensor &rhs)
{
  Tensor res = lhs;
  return res += rhs;
}

Tensor operator-(const Tensor &lhs, const Tensor &rhs)
{
  Tensor res = lhs;
  return res -= rhs;
}

Tensor operator*(const Tensor &lhs, const Tensor &rhs)
{
  Tensor res = lhs;
  return res *= rhs;
}

Tensor operator/(const Tensor &lhs, const Tensor &rhs)
{
  Tensor res = lhs;
  return res /= rhs;
}

Tensor operator*(W lhs, const Tensor &rhs)
{
  Tensor res = rhs;
  return res *= lhs;
}

Tensor operator*(const Tensor &lhs, W rhs)
{
  Tensor res = lhs;
  return res *= rhs;
}

Tensor operator/(const Tensor &lhs, W rhs)
{
  Tensor res = lhs;
  return res /= rhs;
}

Tensor max(const Tensor &t)
{
  auto shape = t.shape;
  Tensor res(shape);

  for (int b = 0; b < shape.batch; b++) {
    W v = 0;
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          v = std::max(v, t(b, c, y, x));
        }
      }
    }
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          res(b, c, y, x) = v;
        }
      }
    }
  }
  return res;
}

Tensor exp(const Tensor &t)
{
  auto shape = t.shape;
  Tensor res(shape);

  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          res(b, c, y, x) = std::exp(t(b, c, y, x));
        }
      }
    }
  }
  return res;
}

Tensor log(const Tensor &t)
{
  auto shape = t.shape;
  Tensor res(shape);

  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          res(b, c, y, x) = std::exp(t(b, c, y, x));
        }
      }
    }
  }
  return res;
}

Tensor sum(const Tensor &t)
{
  auto shape = t.shape;
  Tensor res(shape);

  for (int b = 0; b < shape.batch; b++) {
    W v = 0;
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          v += t(b, c, y, x);
        }
      }
    }
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          res(b, c, y, x) = v;
        }
      }
    }
  }
  return res;
}

Tensor softmax(Tensor x)
{
  x -= max(x);
  x = exp(x);
  return x / sum(x);
}

Tensor mse_loss(const Tensor &y, const Tensor &t)
{
  auto shape = y.shape;
  // auto z = y - t;
  // W sum = 0;
  // for (auto d : z.data) {
  //   sum += d * d;
  // }
  // dump("y", y);
  // dump("t", t);
  // dump("z", z);
  // std::cerr << "loss: " << (sum / shape.cols) << std::endl;

  // ここでは cols がバッチ数
  // return (y - t) / shape.batch;
  return (y - t) / (shape.cols * 0.5);
}

template <typename F>
Tensor fold_tensor(const Tensor &dout, const Shape &newShape, F f)
{
  Tensor t(newShape);
  const Shape &shape = dout.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          t(b, c, y, x) = f(t(b, c, y, x), dout(b, c, y, x));
        }
      }
    }
  }
  return t;
}

Tensor sum_by_axis(const Tensor &dout, const int axis)
{
  auto newShape = fold_by_axis(dout.shape, axis);
  return fold_tensor(dout, newShape, [](W x, W y) { return x + y; });
}

struct Layer {
  virtual Tensor forward(const Tensor &x) = 0;
  virtual Tensor backward(const Tensor &dout) = 0;
  virtual void update() = 0;
};

struct Affine : public Layer {
  Tensor W, b, x;
  Tensor dW, db;

  virtual Tensor forward(const Tensor &x_)
  {
    this->x = x_;
    return dot(x, W) + b;
  }

  virtual Tensor backward(const Tensor &dout)
  {
    auto dx = dot(dout, transpose(W));
    this->dW = dot(transpose(x), dout);
    this->db = sum_by_axis(dout, 0);
    return dx;
  }

  virtual void update()
  {
    W -= 0.1 * dW;
    b -= 0.1 * db;
  }
};

// struct SoftmaxWithLoss : public Layer {
//   Tensor y, t, loss;

//   virtual Tensor forward(const Tensor &x)
//   {
//     this->x = x;
//     return x * W + b;
//   }

//   virtual Tensor backward(const Tensor &dout)
//   {
//     auto dx = dout * transpose(W);
//     this->dW = transpose(x) * dout;
//     this->db = sum_by_axis(dout, 0);
//     return dx;
//   }
// };

void test0()
{
  Tensor t(Shape{2, 1, 1, 2});
  t(0, 0, 0, 0) = 1;
  t(0, 0, 0, 1) = 2;
  // t(0, 0, 1, 0) = 3;
  // t(0, 0, 1, 1) = 4;
  // t(0, 0, 0, 0) = 3;
  // t(0, 0, 0, 1) = 4;
  // t(1, 0, 0, 0) = 5;
  // t(1, 0, 0, 1) = 6;
  // t(1, 0, 1, 0) = 7;
  // t(1, 0, 1, 1) = 8;
  auto res = softmax(t);
  auto shape = res.shape;
  for (int b = 0; b < shape.batch; b++) {
    for (int c = 0; c < shape.channel; c++) {
      for (int y = 0; y < shape.rows; y++) {
        for (int x = 0; x < shape.cols; x++) {
          std::cerr << b << " " << c << " " << y << " " << x << " " << res(b, c, y, x) << std::endl;
        }
      }
    }
  }
}

// https://colab.research.google.com/drive/158kob_kQhX_WTsyyRp8RYkCqpEExGhPg?hl=ja#scrollTo=17_7Bo_vW4Cs
void test1()
{
  Tensor t({1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859});

  Affine affine;
  affine.W = transpose(Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                                -0.0073, 0.2912},
                               {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                                0.2710, 0.1079},
                               {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                                -0.3103, 0.0181}}));

  affine.b = Tensor({-0.0228, 0.1908, -0.1824});

  Tensor target({0.0590, 0.3078, -0.0131});

  auto res = affine.forward(t);
  auto loss = mse_loss(res, target);
  affine.backward(loss);
  affine.update();

  dump("affine.W", transpose(affine.W));
  dump("affine.b", affine.b);
  dump("res", res);
  dump("loss", loss);
  dump("affine.dW", affine.dW);
  dump("affine.db", affine.db);
}

int main()
{
  test1();
  return 0;
}
