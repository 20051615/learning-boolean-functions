// TODO: INCORPORATE A LOGIC MINIMIZER?? ADD A LOGIC MINIMIZER TO SANITIZE FORMULA

#include "ortools/linear_solver/linear_solver.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "lpsvm.h"
#include "formula.h"
#include "id3.h"
#include "ocat.h"
#include "lpsvm.h"
#include "winnow.h"

const std::string FILE_PREFIX = "data/toth/train_data_no_learning/query64_query42_1344n_";
const int NUM_FILES_PER_PREFIX = 19;

int main() {
  std::string first_file_first_line;
  int d;
  int num_formula;
  int num_per_file;

  std::vector<std::vector<int> > x, ys;

  for (int file_index = 0; file_index < NUM_FILES_PER_PREFIX; ++file_index) {
    std::ifstream file;
    file.open(FILE_PREFIX + std::to_string(file_index) + ".txt");
    if (file) {
      std::string line;
      if (file_index == 0) {
        std::getline(file, first_file_first_line);
        std::stringstream ss(first_file_first_line);
        ss >> d;
        ss >> num_formula;
        ss >> num_per_file;
      } else {
        std::getline(file, line);
        if (line != first_file_first_line) {
          LOG(INFO) << "Inconsistent file, skipped";
          file.close();
          continue;
        }
      }
      for (int i = 0; i < num_per_file; ++i) {
        std::vector<int> bool_vector;
        for (int j = 0; j < d; ++j) {
          int atom;
          file >> atom;
          if (atom > 0) bool_vector.push_back(1);
          else bool_vector.push_back(0);
        }
        x.push_back(std::move(bool_vector));

        std::vector<int> output_indicators;
        for (int j = 0; j < num_formula; ++j) {
          int atom;
          file >> atom;
          if (atom > 0) output_indicators.push_back(1);
          else output_indicators.push_back(-1);
        }
        ys.push_back(std::move(output_indicators));
      }

    }

    file.close();
  }

  return EXIT_SUCCESS;
}

// An old main() method to demonstrate/record how to train and predict with the models available.
// Also serves as the definition for the formats of data involved
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
  std::vector<int> y {1, 1, 1, 1, -1, -1, 1, 1};

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

  LOG(INFO) << "WINNOW";
  double weight[d];
  double thresh;
  bool negated[d];
  winnow::train(weight, negated, thresh, x, y, d);
  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << winnow::predict(x[i], weight, negated, thresh, d);
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