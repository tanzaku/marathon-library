
#ifndef LAYER_H
#define LAYER_H

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "tensor.h"

using namespace std;

using W = float;

namespace layer {

struct Layer {
  virtual tensor::Tensor forward(const tensor::Tensor &x) = 0;
  virtual tensor::Tensor backward(const tensor::Tensor &dout) = 0;
  virtual void update(W coeff) = 0;
};

struct Linear : public Layer {
  tensor::Tensor W, b, x;
  tensor::Tensor dW, db;

  virtual ~Linear() {}

  virtual tensor::Tensor forward(const tensor::Tensor &x_)
  {
    this->x = x_;
    return dot(x, W) + b;
  }

  virtual tensor::Tensor backward(const tensor::Tensor &dout)
  {
    ::W numBatch = static_cast<::W>(dout.shape.rows);
    auto dx = dot(dout, transpose(W));
    this->dW = dot(transpose(x), dout) / numBatch;
    this->db = sum_by_axis(dout, 2) / numBatch;
    return dx;
  }

  virtual void update(::W coeff)
  {
    W -= coeff * dW;
    b -= coeff * db;
  }
};

struct Relu : public Layer {
  tensor::Tensor dx;

  virtual ~Relu() {}

  virtual tensor::Tensor forward(const tensor::Tensor &x_)
  {
    auto x = x_;
    this->dx.reshape(x.shape);
    for (int i = 0; i < dx.shape.size(); i++) {
      if (x.data[i] >= 0) {
        this->dx.data[i] = 1;
      } else {
        x.data[i] = 0;
        this->dx.data[i] = 0;
      }
    }
    return x;
  }

  virtual tensor::Tensor backward(const tensor::Tensor &dout)
  {
    return dx * dout;
  }

  virtual void update(::W)
  {
  }
};

struct LeakyRelu : public Layer {
  W negativeSlope;
  tensor::Tensor dx;

  LeakyRelu(W negativeSlope_) : negativeSlope(negativeSlope_) {}
  virtual ~LeakyRelu() {}

  virtual tensor::Tensor forward(const tensor::Tensor &x_)
  {
    auto x = x_;
    this->dx.reshape(x.shape);
    for (int i = 0; i < dx.shape.size(); i++) {
      if (x.data[i] >= 0) {
        this->dx.data[i] = 1;
      } else {
        x.data[i] *= negativeSlope;
        this->dx.data[i] = negativeSlope;
      }
    }
    return x;
  }

  virtual tensor::Tensor backward(const tensor::Tensor &dout)
  {
    assert(dx.shape == dout.shape);
    return dx * dout;
  }

  virtual void update(::W)
  {
  }
};
}

#endif
