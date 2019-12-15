
#ifndef TENSOR_H
#define TENSOR_H

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

using W = float;

namespace tensor {
struct Shape {
  int batch, channel, rows, cols;

  int size() const
  {
    return batch * channel * rows * cols;
  }

  int operator()(int b, int c, int y, int x) const
  {
    b %= batch; // broadcatsing for batch
    c %= channel;
    y %= rows;
    x %= cols;
    // assert(c < channel);
    // assert(y < rows);
    // assert(x < cols);
    return ((b * channel + c) * rows + y) * cols + x;
  }

  int operator()(int y, int x) const
  {
    assert(batch == 1);
    assert(channel == 1);
    return y * cols + x;
  }

  int operator()(int x) const
  {
    assert(batch == 1);
    assert(channel == 1);
    assert(rows == 1);
    return x;
  }
};

Shape operator+(const Shape &lhs, const Shape &rhs)
{
  // assert(lhs.batch % rhs.batch == 0);
  // assert(lhs.channel == rhs.channel);
  // assert(lhs.rows == rhs.rows);
  // assert(lhs.cols == rhs.cols);
  return Shape{std::max(lhs.batch, rhs.batch), std::max(lhs.channel, rhs.channel), std::max(lhs.rows, rhs.rows), std::max(lhs.cols, rhs.cols)};
}

Shape dot(const Shape &lhs, const Shape &rhs)
{
  // assert(lhs.batch % rhs.batch == 0);
  // assert(lhs.cols == rhs.rows);
  // return Shape{lhs.batch, rhs.channel, lhs.rows, rhs.cols};
  assert(lhs.batch == 1);
  assert(rhs.batch == 1);
  assert(lhs.channel == 1);
  assert(rhs.channel == 1);
  return Shape{1, 1, lhs.rows, rhs.cols};
}

Shape transpose(const Shape &s)
{
  // assert(s.batch == 1);
  return Shape{s.batch, s.channel, s.cols, s.rows};
}

int get_by_axis(const Shape &s, int axis)
{
  if (axis == 0) {
    return s.batch;
  }
  if (axis == 1) {
    return s.channel;
  }
  if (axis == 2) {
    return s.rows;
  }
  if (axis == 3) {
    return s.cols;
  }
  throw;
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
  return "tensor::Shape{" + ::to_string(s.batch) + "," + ::to_string(s.channel) + "," + ::to_string(s.rows) + "," + ::to_string(s.cols) + "}";
}




/**
 * 
 */
struct Tensor {
  Shape shape;
  std::vector<W> data;

  Tensor() {}
  Tensor(Shape shape_) : shape(shape_), data(shape.size(), 0) {}
  Tensor(Shape shape_, W val) : shape(shape_), data(shape.size(), val) {}
  Tensor(Shape shape_, std::initializer_list<W> lst1) : shape(shape_), data(lst1) {}
  Tensor(std::initializer_list<W> lst1) : shape({1, 1, 1, static_cast<int>(lst1.size())}), data(lst1) {}
  Tensor(std::initializer_list<initializer_list<W>> lst2) : shape({1, 1, static_cast<int>(lst2.size()), static_cast<int>(std::begin(lst2)->size())})
  {
    data.reserve(shape.size());
    for (auto &lst1 : lst2) {
      std::copy(std::begin(lst1), std::end(lst1), std::back_inserter(data));
    }
  }
  virtual ~Tensor() {}

  void reshape(Shape shape_) {
    shape = shape_;
    data.resize(shape.size());
  }

  W &operator()(int b, int c, int y, int x)
  {
    return data[shape(b, c, y, x)];
  }

  W operator()(int b, int c, int y, int x) const
  {
    return data[shape(b, c, y, x)];
  }

  W &operator()(int y, int x)
  {
    return data[shape(y, x)];
  }

  W operator()(int y, int x) const
  {
    return data[shape(y, x)];
  }

  W &operator()(int x)
  {
    return data[shape(x)];
  }

  W operator()(int x) const
  {
    return data[shape(x)];
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
  std::transform(begin(t.data), end(t.data), std::back_inserter(s), [](W x) { return ::to_string(x); });
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
  auto newShape = dot(lhs.shape, rhs.shape);
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

  assert(shape.batch == 1);
  assert(shape.channel == 1);

  Tensor res(shape);

  for (int y = 0; y < shape.rows; y++) {
    W v = 0;
    for (int x = 0; x < shape.cols; x++) {
      v = std::max(v, t(y, x));
    }
    for (int x = 0; x < shape.cols; x++) {
      res(y, x) = v;
    }
  }
  return res;
}

Tensor exp(const Tensor &t)
{
  auto shape = t.shape;

  assert(shape.batch == 1);
  assert(shape.channel == 1);

  Tensor res(shape);

  for (int y = 0; y < shape.rows; y++) {
    for (int x = 0; x < shape.cols; x++) {
      res(y, x) = std::exp(t(y, x));
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
          res(b, c, y, x) = std::log(t(b, c, y, x));
        }
      }
    }
  }
  return res;
}

// Tensor sum(const Tensor &t)
// {
//   auto shape = t.shape;
//   Tensor res(shape);

//   for (int b = 0; b < shape.batch; b++) {
//     W v = 0;
//     for (int c = 0; c < shape.channel; c++) {
//       for (int y = 0; y < shape.rows; y++) {
//         for (int x = 0; x < shape.cols; x++) {
//           v += t(b, c, y, x);
//         }
//       }
//     }
//     for (int c = 0; c < shape.channel; c++) {
//       for (int y = 0; y < shape.rows; y++) {
//         for (int x = 0; x < shape.cols; x++) {
//           res(b, c, y, x) = v;
//         }
//       }
//     }
//   }
//   return res;
// }

W sum(const Tensor &t)
{
  W v = 0;
  for (W x : t.data) {
    v += x;
  }
  return v;
}

Tensor softmax(Tensor t)
{
  t -= max(t);
  t = exp(t);

  auto shape = t.shape;

  assert(shape.batch == 1);
  assert(shape.channel == 1);

  for (int y = 0; y < shape.rows; y++) {
    W v = 0;
    for (int x = 0; x < shape.cols; x++) {
      v += t(y, x);
    }
    for (int x = 0; x < shape.cols; x++) {
      t(y, x) /= v;
    }
  }
  return t;
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

Tensor mean_by_axis(const Tensor &dout, const int axis)
{
  auto newShape = fold_by_axis(dout.shape, axis);
  W cnt = static_cast<W>(get_by_axis(dout.shape, axis));
  return fold_tensor(dout, newShape, [](W x, W y) { return x + y; }) / cnt;
}

}

#endif
