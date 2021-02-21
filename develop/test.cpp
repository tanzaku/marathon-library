
#include <cassert>
#include <cmath>
#include <memory>
#include <iostream>
#include <vector>
#include "tensor.h"
#include "layer.h"
#include "loss.h"
#include "optimizer.h"

using namespace std;

using W = float;
const W EPS = 1e-4;


void test0()
{
  tensor::Tensor t(tensor::Shape{2, 1, 1, 2});
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
  tensor::Tensor t({1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859});

  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));

  fc1.b = tensor::Tensor({-0.0228, 0.1908, -0.1824});

  tensor::Tensor target({0.0590, 0.3078, -0.0131});

  auto res = fc1.forward(t);
  auto loss = loss::mse_loss(res, target);
  fc1.backward(loss);
  fc1.update(0.1);

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

  tensor::Tensor expectedRes(tensor::Shape{1, 1, 1, 3}, {-0.236099, -0.547132, -0.253642});
  tensor::Tensor expectedLoss(tensor::Shape{1, 1, 1, 3}, {-0.196733, -0.569955, -0.160361});
  tensor::Tensor expectedFc1DW(tensor::Shape{1, 1, 10, 3}, {-0.205940, -0.596629, -0.167866, -0.268343, -0.777418, -0.218733, -0.235607, -0.682578, -0.192049, 0.081034, 0.234764, 0.066053, -0.012138, -0.035166, -0.009894, -0.196831, -0.570240, -0.160442, 0.062423, 0.180847, 0.050883, -0.239148, -0.692837, -0.194935, 0.179263, 0.519343, 0.146121, 0.056246, 0.162950, 0.045847});
  tensor::Tensor expectedFc1Db(tensor::Shape{1, 1, 1, 3}, {-0.196733, -0.569955, -0.160361});

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
  tensor::Tensor t({1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859});

  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228, 0.1908, -0.1824});

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, -0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor target({0.0590, 0.3078});

  auto res = fc1.forward(t);
  res = fc2.forward(res);
  auto loss0 = loss::mse_loss(res, target);
  auto loss = fc2.backward(loss0);
  fc2.update(0.1);
  fc1.backward(loss);
  fc1.update(0.1);

  tensor::Tensor expectedRes(tensor::Shape{1, 1, 1, 2}, {-0.804721, 0.213945});
  tensor::Tensor expectedLoss(tensor::Shape{1, 1, 1, 2}, {-0.863721, -0.093855});
  tensor::Tensor expectedFc1DW(tensor::Shape{1, 1, 10, 3}, {-0.357647, -0.481477, 0.067951, -0.466020, -0.627374, 0.088541, -0.409169, -0.550838, 0.077740, 0.140729, 0.189454, -0.026738, -0.021080, -0.028379, 0.004005, -0.341828, -0.460181, 0.064945, 0.108408, 0.145943, -0.020597, -0.415318, -0.559117, 0.078908, 0.311318, 0.419108, -0.059149, 0.097680, 0.131500, -0.018559});
  tensor::Tensor expectedFc1Db(tensor::Shape{1, 1, 1, 3}, {-0.341657, -0.459952, 0.064913});
  tensor::Tensor expectedFc2DW(tensor::Shape{1, 1, 3, 2}, {0.203924, 0.022159, 0.472570, 0.051351, 0.219076, 0.023805});
  tensor::Tensor expectedFc2Db(tensor::Shape{1, 1, 1, 2}, {-0.863721, -0.093855});

  eq(res, expectedRes, EPS);
  eq(loss0, expectedLoss, EPS);
  eq(fc1.dW, expectedFc1DW, EPS);
  eq(fc1.db, expectedFc1Db, EPS);
  eq(fc2.dW, expectedFc2DW, EPS);
  eq(fc2.db, expectedFc2Db, EPS);
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/14HS4sIKHgWmDQNZ98SNCX0OuFBAUhQok?hl=ja#scrollTo=KBMBvv2U7hf-
 */
void test2_rev()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228, 0.1908, -0.1824});

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, -0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{0.0590, 0.3078},{0.590, 0.03078}});

  std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  auto res = x;
  for (auto& layer: layers) {
    res = layer->forward(res);
  }
  auto loss = loss::mse_loss(res, target);
  for (auto& layer: reversed_layers) {
    loss = layer->backward(loss);
  }
  for (auto& layer: layers) {
    layer->update(opt->getCoeff());
  }

  tensor::Tensor expectedRes(tensor::Shape{1, 1, 2, 2}, {-0.804721,0.213945,-0.046648,0.079922});
  tensor::Tensor expectedLoss(tensor::Shape{1, 1, 2, 2}, {-0.863721,-0.093855,-0.636648,0.049142});
  tensor::Tensor expectedFc1DW(tensor::Shape{1, 1, 10, 3}, {-0.354098,-0.524949,0.047827,-0.272145,-0.377145,0.047363,-0.173672,-0.225294,0.036427,0.092233,0.130188,-0.015097,0.081469,0.135004,-0.005269,-0.186307,-0.255050,0.033689,0.029594,0.033066,-0.008354,-0.172433,-0.222439,0.036670,-0.065967,-0.149816,-0.012060,0.090218,0.132846,-0.012549});
  tensor::Tensor expectedFc1Db(tensor::Shape{1, 1, 1, 3}, {-0.286476,-0.417499,0.041596});
  tensor::Tensor expectedFc2DW(tensor::Shape{1, 1, 3, 2}, {0.040795,0.015801,0.085645,0.037303,0.320499,-0.004381});
  tensor::Tensor expectedFc2Db(tensor::Shape{1, 1, 1, 2}, {-0.750185,-0.022356});

  auto loss0 = loss::mse_loss(res, target);

  // std::cerr << "res " << to_string(res) << std::endl;
  // std::cerr << "loss0 " << to_string(loss0) << std::endl;
  // std::cerr << "fc1.dW " << to_string(fc1.dW) << std::endl;
  // std::cerr << "fc1.db " << to_string(fc1.db) << std::endl;
  // std::cerr << "fc2.dW " << to_string(fc2.dW) << std::endl;
  // std::cerr << "fc2.db " << to_string(fc2.db) << std::endl;

  eq(res, expectedRes, EPS);
  eq(loss0, expectedLoss, EPS);
  eq(fc1.dW, expectedFc1DW, EPS);
  eq(fc1.db, expectedFc1Db, EPS);
  eq(fc2.dW, expectedFc2DW, EPS);
  eq(fc2.db, expectedFc2Db, EPS);
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/14HS4sIKHgWmDQNZ98SNCX0OuFBAUhQok?hl=ja#scrollTo=KBMBvv2U7hf-
 */
void test2_learn()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228, 0.1908, -0.1824});

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, -0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{0.0590, 0.3078},{0.590, 0.03078}});

  std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  for (int i = 0; i < 100; i++) {
    auto res = x;
    for (auto& layer: layers) {
      res = layer->forward(res);
    }
    auto loss = loss::mse_loss(res, target);
    for (auto& layer: reversed_layers) {
      loss = layer->backward(loss);
    }
    for (auto& layer: layers) {
      layer->update(opt->getCoeff());
    }
    std::cerr << i << " " << loss::calc_mse_loss(res, target) << std::endl;
  }
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/14HS4sIKHgWmDQNZ98SNCX0OuFBAUhQok?hl=ja#scrollTo=KBMBvv2U7hf-
 */
void test2_relu()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228,  0.1908, 0.1824});

  layer::Relu relu1;

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, 0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{0.0590, 0.3078},{0.590, 0.03078}});

  std::vector<layer::Layer*> layers {&fc1, &relu1, &fc2};
  // std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  for (int i = 0; i < 100; i++) {
    auto res = x;
    for (auto& layer: layers) {
      res = layer->forward(res);
    }
    auto loss = loss::mse_loss(res, target);
    for (auto& layer: reversed_layers) {
      loss = layer->backward(loss);
    }
    for (auto& layer: layers) {
      layer->update(opt->getCoeff());
    }
    // std::cerr << i << " " << loss::calc_mse_loss(res, target) << std::endl;

    tensor::Tensor expectedRes(tensor::Shape{1, 1, 2, 2}, {-0.423836,0.052778,-0.078459,-0.085693});
    tensor::Tensor expectedLoss(tensor::Shape{1, 1, 2, 2}, {-0.482836,-0.255022,-0.668459,-0.116473});
    tensor::Tensor expectedFc1DW(tensor::Shape{1, 1, 10, 3}, {-0.206143,-0.259642,-0.021226,-0.046027,-0.057972,-0.027658,0.036357,0.045792,-0.024284,0.025720,0.032395,0.008352,0.108213,0.136297,-0.001251,-0.018103,-0.022802,-0.020287,-0.028944,-0.036455,0.006434,0.041430,0.052182,-0.024649,-0.260657,-0.328304,0.018476,0.048666,0.061296,0.005797});
    tensor::Tensor expectedFc1Db(tensor::Shape{1, 1, 1, 3}, {-0.136014,-0.171313,-0.020277});
    tensor::Tensor expectedFc2DW(tensor::Shape{1, 1, 3, 2}, {-0.064223,-0.011190,-0.158166,-0.027559,-0.026835,-0.014174});
    tensor::Tensor expectedFc2Db(tensor::Shape{1, 1, 1, 2}, {-0.575647,-0.185747});

    auto loss0 = loss::mse_loss(res, target);

    // std::cerr << "res " << to_string(res) << std::endl;
    // std::cerr << "loss0 " << to_string(loss0) << std::endl;
    // std::cerr << "fc1.dW " << to_string(fc1.dW) << std::endl;
    // std::cerr << "fc1.db " << to_string(fc1.db) << std::endl;
    // std::cerr << "fc2.dW " << to_string(fc2.dW) << std::endl;
    // std::cerr << "fc2.db " << to_string(fc2.db) << std::endl;

    eq(res, expectedRes, EPS);
    eq(loss0, expectedLoss, EPS);
    eq(fc1.dW, expectedFc1DW, EPS);
    eq(fc1.db, expectedFc1Db, EPS);
    eq(fc2.dW, expectedFc2DW, EPS);
    eq(fc2.db, expectedFc2Db, EPS);

    break;
  }
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/14HS4sIKHgWmDQNZ98SNCX0OuFBAUhQok?hl=ja#scrollTo=KBMBvv2U7hf-
 */
void test2_linear_relu_learn()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228,  0.1908, 0.1824});

  layer::Relu relu1;

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, 0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{0.0590, 0.3078},{0.590, 0.03078}});

  std::vector<layer::Layer*> layers {&fc1, &relu1, &fc2};
  // std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  for (int i = 0; i < 100; i++) {
    auto res = x;
    for (auto& layer: layers) {
      res = layer->forward(res);
    }
    auto loss = loss::mse_loss(res, target);
    for (auto& layer: reversed_layers) {
      loss = layer->backward(loss);
    }
    for (auto& layer: layers) {
      layer->update(opt->getCoeff());
    }
    std::cerr << i << " " << loss::calc_mse_loss(res, target) << std::endl;

    // tensor::Tensor expectedRes(tensor::Shape{1, 1, 2, 2}, {-0.423836,0.052778,-0.078459,-0.085693});
    // tensor::Tensor expectedLoss(tensor::Shape{1, 1, 2, 2}, {-0.482836,-0.255022,-0.668459,-0.116473});
    // tensor::Tensor expectedFc1DW(tensor::Shape{1, 1, 10, 3}, {-0.206143,-0.259642,-0.021226,-0.046027,-0.057972,-0.027658,0.036357,0.045792,-0.024284,0.025720,0.032395,0.008352,0.108213,0.136297,-0.001251,-0.018103,-0.022802,-0.020287,-0.028944,-0.036455,0.006434,0.041430,0.052182,-0.024649,-0.260657,-0.328304,0.018476,0.048666,0.061296,0.005797});
    // tensor::Tensor expectedFc1Db(tensor::Shape{1, 1, 1, 3}, {-0.136014,-0.171313,-0.020277});
    // tensor::Tensor expectedFc2DW(tensor::Shape{1, 1, 3, 2}, {-0.064223,-0.011190,-0.158166,-0.027559,-0.026835,-0.014174});
    // tensor::Tensor expectedFc2Db(tensor::Shape{1, 1, 1, 2}, {-0.575647,-0.185747});

    // auto loss0 = loss::mse_loss(res, target);

    // std::cerr << "res " << to_string(res) << std::endl;
    // std::cerr << "loss0 " << to_string(loss0) << std::endl;
    // std::cerr << "fc1.dW " << to_string(fc1.dW) << std::endl;
    // std::cerr << "fc1.db " << to_string(fc1.db) << std::endl;
    // std::cerr << "fc2.dW " << to_string(fc2.dW) << std::endl;
    // std::cerr << "fc2.db " << to_string(fc2.db) << std::endl;

    // eq(res, expectedRes, EPS);
    // eq(loss0, expectedLoss, EPS);
    // eq(fc1.dW, expectedFc1DW, EPS);
    // eq(fc1.db, expectedFc1Db, EPS);
    // eq(fc2.dW, expectedFc2DW, EPS);
    // eq(fc2.db, expectedFc2Db, EPS);

    // break;
  }
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/1Dj5jNIlRbC6HOhED_ZjesYJa9bExHiMI?hl=ja#scrollTo=2jz3tFwPKEpi
 */
void test2_linear_softmax()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228,  0.1908, 0.1824});

  layer::LeakyRelu relu1(0.01);

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, 0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{1, 0},{0, 1}});

  std::vector<layer::Layer*> layers {&fc1, &relu1, &fc2};
  // std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  for (int i = 0; i < 100; i++) {
    auto res = x;
    for (auto& layer: layers) {
      res = layer->forward(res);
    }
    auto loss = loss::softmax_loss(tensor::softmax(res), target);
    for (auto& layer: reversed_layers) {
      loss = layer->backward(loss);
    }
    for (auto& layer: layers) {
      layer->update(opt->getCoeff());
    }
    // std::cerr << i << " " << loss::cross_entropy_error(tensor::softmax(res), target) << std::endl;

    auto loss0 = loss::softmax_loss(tensor::softmax(res), target);
    tensor::Tensor expectedRes(tensor::Shape{1,1,2,2}, {-0.427820,0.054034,-0.078316,-0.086437});
    tensor::Tensor expectedLoss(tensor::Shape{1,1,2,2}, {-0.618185,0.618185,0.502030,-0.502030});
    tensor::Tensor expectedFc1W(tensor::Shape{1,1,10,3}, {0.148839,-0.046117,0.140975,-0.167140,-0.176723,0.102566,-0.056362,-0.230941,-0.192047,0.064838,0.160419,0.285079,-0.200540,0.200391,0.270272,-0.044916,0.002862,-0.006903,0.082695,0.142367,-0.100862,0.087429,0.141479,-0.326016,-0.017127,0.228904,-0.301766,0.293006,0.115637,0.020706});
    tensor::Tensor expectedFc1b(tensor::Shape{1,1,1,3}, {-0.027835,0.169231,0.173267});
    tensor::Tensor expectedFc2W(tensor::Shape{1,1,3,2}, {0.371804,0.178496,0.553552,-0.292352,-0.044489,0.246389});
    tensor::Tensor expectedFc2b(tensor::Shape{1,1,1,2}, {-0.412692,0.019192});
    tensor::Tensor expectedFc1DW(tensor::Shape{1,1,10,3}, {0.076610,0.328167,0.095255,0.016396,0.070233,0.125342,-0.014379,-0.061594,0.110473,-0.009382,-0.040189,-0.037786,-0.040599,-0.173912,0.006276,0.006158,0.026376,0.092025,0.011048,0.047325,-0.029376,-0.016292,-0.069788,0.112159,0.098272,0.420960,-0.085335,-0.018062,-0.077369,-0.026058});
    tensor::Tensor expectedFc1Db(tensor::Shape{1,1,1,3}, {0.050353,0.215694,0.091331});
    tensor::Tensor expectedFc2DW(tensor::Shape{1,1,3,2}, {0.048963,-0.048963,0.120478,-0.120478,-0.035106,0.035106});
    tensor::Tensor expectedFc2Db(tensor::Shape{1,1,1,2}, {-0.058078,0.058078});

    // std::cerr << "tensor::Tensor expectedRes(" << to_string(res) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedLoss(" << to_string(loss0) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc1W(" << to_string(fc1.W) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc1b(" << to_string(fc1.b) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc2W(" << to_string(fc2.W) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc2b(" << to_string(fc2.b) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc1DW(" << to_string(fc1.dW) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc1Db(" << to_string(fc1.db) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc2DW(" << to_string(fc2.dW) << ");" << std::endl;
    // std::cerr << "tensor::Tensor expectedFc2Db(" << to_string(fc2.db) << ");" << std::endl;

    eq(res, expectedRes, EPS);
    eq(loss0, expectedLoss, EPS);
    eq(fc1.W, expectedFc1W, EPS);
    eq(fc1.b, expectedFc1b, EPS);
    eq(fc2.W, expectedFc2W, EPS);
    eq(fc2.b, expectedFc2b, EPS);
    eq(fc1.dW, expectedFc1DW, EPS);
    eq(fc1.db, expectedFc1Db, EPS);
    eq(fc2.dW, expectedFc2DW, EPS);
    eq(fc2.db, expectedFc2Db, EPS);

    // if (i >= 3)
      break;
  }
}

/**
 * PyTorchの挙動と合わせてデバッグ
 * https://colab.research.google.com/drive/1Dj5jNIlRbC6HOhED_ZjesYJa9bExHiMI?hl=ja#scrollTo=2jz3tFwPKEpi
 */
void test2_linear_softmax_learn()
{
  layer::Linear fc1;
  fc1.W = transpose(tensor::Tensor({{0.1565, -0.1655, -0.0578, 0.0639, -0.2046, -0.0443, 0.0838, 0.0858,
                             -0.0073, 0.2912},
                            {-0.0133, -0.1697, -0.2371, 0.1564, 0.1830, 0.0055, 0.1471, 0.1345,
                             0.2710, 0.1079},
                            {0.1505, 0.1151, -0.1810, 0.2813, 0.2709, 0.0023, -0.1038, -0.3148,
                             -0.3103, 0.0181}}));
  fc1.b = tensor::Tensor({-0.0228,  0.1908, 0.1824});

  layer::LeakyRelu relu1(0.01);

  layer::Linear fc2;
  fc2.W = transpose(tensor::Tensor({
      {0.3767, 0.5656, -0.0480},
      {0.1736, -0.3044, 0.2499},
  }));
  fc2.b = tensor::Tensor({-0.4185, 0.0250});

  tensor::Tensor x({{1.0468, 1.3640, 1.1976, -0.4119, 0.0617, 1.0005, -0.3173, 1.2156,
            -0.9112, -0.2859},
            {1.5156,  0.3384, -0.2673, -0.1891, -0.7956,  0.1331,  0.2128, -0.3046,
         1.9164, -0.3578}});
  tensor::Tensor target({{1, 0},{0, 1}});

  std::vector<layer::Layer*> layers {&fc1, &relu1, &fc2};
  // std::vector<layer::Layer*> layers {&fc1, &fc2};
  auto reversed_layers = layers;
  std::reverse(begin(reversed_layers), end(reversed_layers));
  auto opt = std::make_shared<optimizer::SGD>(0.1);

  for (int i = 0; i < 100; i++) {
    auto res = x;
    for (auto& layer: layers) {
      res = layer->forward(res);
    }
    auto loss = loss::softmax_loss(tensor::softmax(res), target);
    for (auto& layer: reversed_layers) {
      loss = layer->backward(loss);
    }
    for (auto& layer: layers) {
      layer->update(opt->getCoeff());
    }
    std::cerr << i << " " << loss::cross_entropy_error(tensor::softmax(res), target) << std::endl;
  }
}

int main()
{
  // test1();
  // test2();
  // test2_rev();
  // test2_learn();
  // test2_relu();
  // test2_linear_relu_learn();
  test2_linear_softmax();
  test2_linear_softmax_learn();
  return 0;
}
