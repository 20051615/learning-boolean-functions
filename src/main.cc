#include "ortools/linear_solver/linear_solver.h"

#include <vector>

#include "lpsvm.h"
#include "formula.h"
#include "id3.h"
#include "ocat.h"

// DEAL WITH THIS
#include "svm_kernel.cc"

int main() {
  std::vector<std::vector<int> > x {
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0},
    {1, 1, 1},
  };
  int y[] = {1, -1, 1, -1, 1, 1, 1, 1};
  
  LOG(INFO) << "LPSVM";
  int m = x.size();
  int d = x[0].size();
  double b = 0;
  double a[m];
  lpsvm::train(m, d, x, y, a, b);
  
  LOG(INFO) << "Predicting using kernel:";
  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << lpsvm::predict(x[i], m, d, x, y, a, b);
  }

  LOG(INFO) << "Predicting using weight vector:";
  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << lpsvm::w_predict(x[i], m, d, x, y, a, b);
  }

  std::vector<std::tuple<int, double> > w = lpsvm::w(m, d, x, y, a);
  LOG(INFO) << "Size of w: " << w.size();
  for (int i = 0; i < w.size(); ++i) {
    LOG(INFO) << "w[" << i << "]: " << std::get<0>(w[i]) << " " << std::get<1>(w[i]);
  }

  for (int i = 0; i < m; ++i) {
    LOG(INFO) << "a[" << i << "]: " << a[i];
  }

  LOG(INFO) << "b: " << b;

  LOG(INFO) << "OCAT";
  std::vector<std::vector<int> > formula = ocat::train(x, y);
  LOG(INFO) << "OCAT CNF: " << to_string(true, formula);
  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << eval(true, formula, x[i]);
  }

  LOG(INFO) << "ID3";
  id3::Tree decision_tree(x, y);
  formula = decision_tree.getFormula(true);
  LOG(INFO) << "ID3 CNF: " << to_string(true, formula);

  formula = decision_tree.getFormula(false);
  LOG(INFO) << "ID3 DNF: " << to_string(false, formula);

  for (const std::vector<int> &sample : x) {
    LOG(INFO) << decision_tree.predict(sample);
  }

  return EXIT_SUCCESS;
}
