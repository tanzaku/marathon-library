
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "tensor.h"

using namespace std;

namespace optimizer {
  struct Optimizer {
    virtual W getCoeff() = 0;
  };

  struct SGD : public Optimizer {
    W a;

    SGD() {}
    SGD(W a_) : a(a_) {}
    virtual ~SGD() {}

    virtual W getCoeff() {
      return a;
    }
  };
}

#endif
