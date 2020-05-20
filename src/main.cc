// TODO: INCORPORATE A LOGIC MINIMIZER?? ADD A LOGIC MINIMIZER TO SANITIZE FORMULA

#include "ortools/linear_solver/linear_solver.h"

#include <vector>

#include "lpsvm.h"
#include "formula.h"
#include "id3.h"
#include "ocat.h"
#include "lpsvm.h"

int pos_neg_digit(int n, int j) {
  return 2 * ((n & (1 << j)) >> j) - 1;
}

void fill_y(int n, int y[]) {
  for (int i = 15; i >= 0; --i) y[i] = pos_neg_digit(n, 15 - i);
}

int main() {
  std::vector<std::vector<int> > x {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 0},
    {0, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 1, 0},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 0, 1},
    {1, 1, 1, 0},
    {1, 1, 1, 1}
  };

  int m = x.size();
  int d = 4;
  double b = 0;
  double a[m];
  int y[16];
  int approx_accuracy_average = 0;
  int accuracy_average = 0;
  for (int n = 0; n < 65536; ++n) {
    fill_y(n, y);
    
    lpsvm::train(m, d, x, y, a, b);
    std::vector<std::vector<int> > formula_approx = lpsvm::approx_formula(m, d, x, y, a);


    for (int i = 0; i < 16; ++i) {
      if (lpsvm::predict(x[i], m, d, x, y, a, b) == y[i]) accuracy_average += 1;
      if (eval(false, formula_approx, x[i]) == (y[i] == 1)) approx_accuracy_average += 1;
    }

  }
  LOG(INFO) << "accuracy_avg: " << accuracy_average / (double)(65536 * 16);
  LOG(INFO) << "approx_accuracy_avg: " << approx_accuracy_average / (double)(65536 * 16);

  return EXIT_SUCCESS;
}
/*
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
  int y[] = {-1, -1, 1, 1, -1, -1, 1, 1};
  
  LOG(INFO) << "LPSVM";
  int m = x.size();
  int d = x[0].size();
  double b = 0;
  double a[m];
  lpsvm::train(m, d, x, y, a, b);
  
  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << lpsvm::predict(x[i], m, d, x, y, a, b);
  }

  std::vector<std::vector<int> > approx_formula = lpsvm::approx_formula(m, d, x, y, a);
  LOG(INFO) << "SVM APPROX DNF: " << to_string(false, approx_formula);

  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << eval(false, approx_formula, x[i]);
  }

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
*/