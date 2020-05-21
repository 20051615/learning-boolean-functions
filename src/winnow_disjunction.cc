#include "winnow.h"

#include <vector>

namespace winnow {

void train(double weight[], bool negated[], double &thresh, const std::vector<std::vector<int> > &x, const std::vector<int> &y, int d) {
  for (int i = 0; i < d; ++i) {
    negated[i] = false;
    weight[i] = 1;
  }
  thresh = (double) d / 2;

  int example_index = 0;
  while (example_index < x.size() && y[example_index] == 1) ++example_index;
  if (example_index == x.size()) {
    thresh = -1;
    return;
  }

  for (int i = 0; i < d; ++i) {
    if (x[example_index][i] == 1) negated[i] = true;
  }

  for (example_index = 0; example_index < x.size(); ++example_index) {
    int prediction = predict(x[example_index], weight, negated, thresh, d);
    
    if (y[example_index] == -1 && prediction == 1) {
      for (int j = 0; j < d; ++j) {
        if (x[example_index][j] == (negated[j] ? 0 : 1)) weight[j] = 0;
      }
    } else if (y[example_index] == 1 && prediction == -1) {
      for (int j = 0; j < d; ++j) {
        if (x[example_index][j] == (negated[j] ? 0 : 1)) weight[j] *= 2;
      }
    }
  }
}

int predict(const std::vector<int> &to_predict, double weight[], bool negated[], double &thresh, int d) {
  double sum = 0;
  for (int i = 0; i < d; ++i) {
    sum += weight[i] * (negated[i] ? 1 - to_predict[i] : to_predict[i]);
  }
  if (sum > thresh) return 1;
  else return -1;
}

}
