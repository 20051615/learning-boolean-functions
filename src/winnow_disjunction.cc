#include "winnow.h"

#include <vector>

namespace winnow {

void train(double weight[], const std::vector<std::vector<int> > &x, int y[], int d) {
  for (int i = 0; i < d; ++i) weight[i] = 1;

  for (int i = 0; i < x.size(); ++i) {
    int prediction = predict(x[i], weight, d);
    
    if (y[i] == -1 && prediction == 1) {
      for (int j = 0; j < d; ++j) {
        if (x[i][j] == 1) weight[j] = 0;
      }
    } else if (y[i] == 1 && prediction == -1) {
      for (int j = 0; j < d; ++j) {
        if (x[i][j] == 1) weight[j] *= 2;
      }
    }
  }
}

int predict(const std::vector<int> &to_predict, double weight[], int d) {
  double sum = 0, thresh = (double) d / 2;
  for (int i = 0; i < d; ++i) {
    sum += weight[i] * to_predict[i];
  }
  if (sum > thresh) return 1;
  else return -1;
}

}