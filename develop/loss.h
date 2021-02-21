
#ifndef LOSS_H
#define LOSS_H

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "tensor.h"

using namespace std;

namespace loss {
  tensor::Tensor mse_loss(const tensor::Tensor &y, const tensor::Tensor &t)
  {
    assert(y.shape == t.shape);

    // ここでは cols がバッチ数
    // return (y - t) / shape.batch;
    // auto meanShape = tensor::Shape { 1, 1, 1, y.shape.rows };
    // std::cerr << "y " << to_string(y) << std::endl;
    // std::cerr << "t " << to_string(t) << std::endl;
    // std::cerr << "meanShape " << to_string(meanShape) << std::endl;
    // tensor::Tensor mean(meanShape, 1.0 / y.shape.rows);
    // return tensor::dot(mean, y - t) / (shape.cols * 0.5);
    return (y - t) / (y.shape.cols * 0.5);
  }

  W calc_mse_loss(const tensor::Tensor &y, const tensor::Tensor &t)
  {
    assert(y.shape == t.shape);

    auto v = y;
    v -= t;
    return tensor::sum(tensor::dot(v, transpose(v)) / 2);
  }

  W cross_entropy_error(const tensor::Tensor &y, const tensor::Tensor &t)
  {
    // 入力
    // batch : 1
    // channel : 1
    // rows : バッチ数
    // cols : ラベル数

    // tは正しいラベルが1で、それ以外は0
    assert(y.shape == t.shape);
    W v = 0;
    for (int b = 0; b < y.shape.rows; b++) {
      for (int c = 0; c < y.shape.cols; c++) {
        if (t(b, c) > 0.5) {
          v += std::log(y(b, c) + 1e-7);
          break;
        }
      }
    }
    return -v / y.shape.rows;
  }

  tensor::Tensor softmax_loss(const tensor::Tensor &y, const tensor::Tensor &t)
  {
    assert(y.shape == t.shape);
    // return (y - t) / y.shape.rows;
    return y - t;
  }
}

#endif
