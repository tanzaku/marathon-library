
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

using W = float;
const W EPS = 1e-4;

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

bool operator==(const Shape &lhs, const Shape &rhs)
{
  return lhs.batch == rhs.batch &&
         lhs.channel == rhs.channel &&
         lhs.rows == rhs.rows &&
         lhs.cols == rhs.cols;
}

bool operator!=(const Shape &lhs, const Shape &rhs)
{
  return !(lhs == rhs);
}

std::string to_string(Shape s)
{
  return "Shape{" + to_string(s.batch) + "," + to_string(s.channel) + "," + to_string(s.rows) + "," + to_string(s.cols) + "}";
}

/**
 * 
 */
struct Tensor {
  Shape shape;
  std::vector<W> data;

  Tensor() {}
  Tensor(Shape shape_) : shape(shape_), data(shape.size(), 0) {}
  Tensor(Shape shape_, std::initializer_list<W> lst1) : shape(shape_), data(lst1) {}
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

std::string to_string(const Tensor &t)
{
  auto quote = [](std::string s) { return "{" + s + "}"; };

  auto join = [](const vector<std::string> &vec, std::string delimiter) {
    std::string res;
    for (int i = 0; i < (int)vec.size(); i++) {
      if (i) res += delimiter;
      res += vec[i];
    }
    return res;
  };

  vector<std::string> s;
  std::transform(begin(t.data), end(t.data), std::back_inserter(s), [](W x) { return to_string(x); });
  return to_string(t.shape) + ", " + quote(join(s, ","));
}

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

void eq(const Tensor &lhs, const Tensor &rhs, const W EPS_)
{
  assert(lhs.shape == rhs.shape);
  for (int i = 0; i < (int)lhs.data.size(); i++) {
    assert(std::abs(lhs.data[i] - rhs.data[i]) <= EPS_);
  }
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

struct Linear : public Layer {
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

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/158kob_kQhX_WTsyyRp8RYkCqpEExGhPg?hl=ja#scrollTo=17_7Bo_vW4Cs
 */
void test1()
{
  Tensor t({1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859});

  Linear fc1;
  fc1.W = transpose(Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));

  fc1.b = Tensor({-0.0228, 0.1908, -0.1824});

  Tensor target({0.0590, 0.3078, -0.0131});

  auto res = fc1.forward(t);
  auto loss = mse_loss(res, target);
  fc1.backward(loss);
  fc1.update();

  // dump("fc1.W", transpose(fc1.W));
  // dump("fc1.b", fc1.b);
  // dump("res", res);
  // dump("loss", loss);
  // dump("fc1.dW", fc1.dW);
  // dump("fc1.db", fc1.db);

  // std::cerr << "res " << to_string(res) << std::endl;
  // std::cerr << "loss " << to_string(loss) << std::endl;
  // std::cerr << "fc1.W " << to_string(fc1.W) << std::endl;
  // std::cerr << "fc1.b " << to_string(fc1.b) << std::endl;
  // std::cerr << "fc1.dW " << to_string(fc1.dW) << std::endl;
  // std::cerr << "fc1.db " << to_string(fc1.db) << std::endl;

  Tensor expectedRes(Shape{1, 1, 1, 3}, {-0.236099, -0.547132, -0.253642});
  Tensor expectedLoss(Shape{1, 1, 1, 3}, {-0.196733, -0.569955, -0.160361});
  Tensor expectedFc1DW(Shape{1, 1, 10, 3}, {-0.205940, -0.596629, -0.167866, -0.268343, -0.777418, -0.218733, -0.235607, -0.682578, -0.192049, 0.081034, 0.234764, 0.066053, -0.012138, -0.035166, -0.009894, -0.196831, -0.570240, -0.160442, 0.062423, 0.180847, 0.050883, -0.239148, -0.692837, -0.194935, 0.179263, 0.519343, 0.146121, 0.056246, 0.162950, 0.045847});
  Tensor expectedFc1Db(Shape{1, 1, 1, 3}, {-0.196733, -0.569955, -0.160361});

  eq(res, expectedRes, EPS);
  eq(loss, expectedLoss, EPS);
  eq(fc1.dW, expectedFc1DW, EPS);
  eq(fc1.db, expectedFc1Db, EPS);
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/14HS4sIKHgWmDQNZ98SNCX0OuFBAUhQok?hl=ja#scrollTo=KBMBvv2U7hf-
 */
void test2()
{
  Tensor t({1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859});

  Linear fc1;
  fc1.W = transpose(Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = Tensor({-0.0228, 0.1908, -0.1824});

  Linear fc2;
  fc2.W = transpose(Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, -0.2499},
  }));
  fc2.b = Tensor({-0.4185, 0.0250});

  Tensor target({0.0590, 0.3078});

  auto res = fc1.forward(t);
  res = fc2.forward(res);
  auto loss0 = mse_loss(res, target);
  auto loss = fc2.backward(loss0);
  fc2.update();
  fc1.backward(loss);
  fc1.update();

  // dump("fc1.W", transpose(fc1.W));
  // dump("fc1.b", fc1.b);
  // dump("fc2.W", transpose(fc2.W));
  // dump("fc2.b", fc2.b);
  // dump("res", res);
  // dump("loss", loss);
  // dump("fc1.dW", fc1.dW);
  // dump("fc1.db", fc1.db);
  // dump("fc2.dW", fc2.dW);
  // dump("fc2.db", fc2.db);

  // // std::cerr << "fc1.W " << to_string(fc1.W) << std::endl;
  // // std::cerr << "fc1.b " << to_string(fc1.b) << std::endl;
  // std::cerr << "res " << to_string(res) << std::endl;
  // std::cerr << "loss " << to_string(loss0) << std::endl;
  // std::cerr << "fc1.dW " << to_string(fc1.dW) << std::endl;
  // std::cerr << "fc1.db " << to_string(fc1.db) << std::endl;
  // // std::cerr << "fc2.W " << to_string(fc2.W) << std::endl;
  // // std::cerr << "fc2.b " << to_string(fc2.b) << std::endl;
  // std::cerr << "fc2.dW " << to_string(fc2.dW) << std::endl;
  // std::cerr << "fc2.db " << to_string(fc2.db) << std::endl;

  Tensor expectedRes(Shape{1, 1, 1, 2}, {-0.804721, 0.213945});
  Tensor expectedLoss(Shape{1, 1, 1, 2}, {-0.863721, -0.093855});
  Tensor expectedFc1DW(Shape{1, 1, 10, 3}, {-0.357647, -0.481477, 0.067951, -0.466020, -0.627374, 0.088541, -0.409169, -0.550838, 0.077740, 0.140729, 0.189454, -0.026738, -0.021080, -0.028379, 0.004005, -0.341828, -0.460181, 0.064945, 0.108408, 0.145943, -0.020597, -0.415318, -0.559117, 0.078908, 0.311318, 0.419108, -0.059149, 0.097680, 0.131500, -0.018559});
  Tensor expectedFc1Db(Shape{1, 1, 1, 3}, {-0.341657, -0.459952, 0.064913});
  Tensor expectedFc2DW(Shape{1, 1, 3, 2}, {0.203924, 0.022159, 0.472570, 0.051351, 0.219076, 0.023805});
  Tensor expectedFc2Db(Shape{1, 1, 1, 2}, {-0.863721, -0.093855});

  eq(res, expectedRes, EPS);
  eq(loss0, expectedLoss, EPS);
  eq(fc1.dW, expectedFc1DW, EPS);
  eq(fc1.db, expectedFc1Db, EPS);
  eq(fc2.dW, expectedFc2DW, EPS);
  eq(fc2.db, expectedFc2Db, EPS);
}

int main()
{
  test1();
  // test2();
  return 0;
}
