// This file concerns input-feature space manipulations, with respect to the "DNF kernel" as described by Ken Sadohara
// 2 is the value used for "D" in the paper

// TODO: i needs an integer type capable of storing 3^d

const double EPSILON = 1.0e-6;
const double W_TRUNC_FACTOR = 0.2;

#include "lpsvm.h"

#include <vector>
#include <tuple>
#include <algorithm>
#include <cmath>

namespace lpsvm {

int pow_3(int d) {
  int result = 1;
  for (int k = 0; k < d; ++k) result *= 3;
  return result;
}

bool is_small(double a) {
  return std::abs(a) < EPSILON;
}

int L(int x, int p) {
  if (p == 1) return x;
  else if (p == 0) return 1 - x;
  else return 1;
}

// This essentially performs the conversion of integer i into base-3, stored in p.
// i should not be 0.
void idx_inverse(int i, int p_store[], int d) {
  i -= 1;
  for (int index = d - 1; index >= 0; --index) {
    p_store[index] = i % 3;
    i /= 3;
  }
}

// this performs the conversion of boolean vector x from input space to feature space, with respect to the feature space's ith dimension
int phi_i(const std::vector<int> &x, int i, int d) {
  int p[d];
  idx_inverse(i, p, d);
  for (int k = 0; k < d; ++k) {
    if (L(x[k], p[k]) == 0) return 0;
  }
  return 1;
}

std::vector<std::tuple<int, double> > w(int m, int d, const std::vector<std::vector<int> > &x, int y[], double a[]) {
  std::vector<std::tuple<int, double>> result;
  for (int k = 1; k < pow_3(d); ++k) {
    double weight = 0;
    for (int i = 0; i < m; ++i) {
      if (is_small(a[i])) continue;
      if (phi_i(x[i], k, d)) weight += a[i] * y[i];
    }
    if (!is_small(weight))
      result.push_back(std::make_tuple(k, weight));
  }
  return result;
}

bool w_comparator(const std::tuple<int, double> &i, const std::tuple<int, double> &j) {
  return std::abs(std::get<1>(i)) > std::abs(std::get<1>(j));
}

std::vector<std::vector<int> > approx_formula(int m, int d, const std::vector<std::vector<int> > &x, int y[], double a[]) {
  std::vector<std::vector<int> > result;
  std::vector<std::tuple<int, double> > weights = w(m, d, x, y, a);
  int post_trunc_size = weights.size() * W_TRUNC_FACTOR;
  std::sort(weights.begin(), weights.end(), w_comparator);

  int p[d];
  for (int i = 0; i < post_trunc_size; ++i) {
    int k = std::get<0>(weights[i]);
    idx_inverse(k, p, d);
    double weight = std::get<1>(weights[i]);

    if (weight > 0) {
      std::vector<int> clause;
      for (int q = 0; q < d; ++q) {
        if (p[q] != 2) {
          int index = (q + 1) * (2 * p[q] - 1);
          clause.push_back(index);
        }
      }
      result.push_back(std::move(clause));
    } else {
      for (int q = 0; q < d; ++q) {
        if (p[q] != 2) {
          std::vector<int> clause;
          int index = (q + 1) * (1 - 2 * p[q]);
          clause.push_back(index);
          result.push_back(std::move(clause));
        }
      }
    }
  }
  return result;
}

}