// TODO: INCORPORATE A LOGIC MINIMIZER?? ADD A LOGIC MINIMIZER TO SANITIZE FORMULA

#include "ortools/linear_solver/linear_solver.h"

#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <random>

auto rng = std::default_random_engine {};

#include "lpsvm.h"
#include "formula.h"
#include "id3.h"
#include "ocat.h"
#include "lpsvm.h"
#include "winnow.h"

const std::string FILE_PREFIX = "data/toth/train_data_no_learning/small-dyn-partition-fixpoint-10_";
const int NUM_FILES_PER_PREFIX = 19;

const double ML_SPLIT = 0.5;
const int NUM_FORMULAS_TO_TEST = 30;

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

  int num_samples = x.size();
  std::vector<int> sample_indices;
  for (int i = 0; i < num_samples; ++i) {
    sample_indices.push_back(i);
  }
  int training_size = num_samples * ML_SPLIT;

  std::vector<double> lpsvm_acc, ocat_acc, id3_acc, winnow_acc;

  for (int formula_i = 0; formula_i < std::min(num_formula, NUM_FORMULAS_TO_TEST); ++formula_i) {
    std::shuffle(std::begin(sample_indices), std::end(sample_indices), rng);
    std::vector<std::vector<int> > training_x, testing_x;
    std::vector<int> training_y, testing_y;

    for (int i = 0; i < training_size; ++i) {
      training_x.push_back(x[sample_indices[i]]);
      training_y.push_back(ys[sample_indices[i]][formula_i]);
    }

    for (int i = training_size; i < num_samples; ++i) {
      testing_x.push_back(x[sample_indices[i]]);
      testing_y.push_back(ys[sample_indices[i]][formula_i]);
    }

    LOG(INFO) << "Index of formula about to be learned: " << formula_i;

    int correct_count = 0;
    int m = training_x.size();
    double b;
    double a[m];

    bool formula_solvable = lpsvm::train(m, d, training_x, training_y, a, b);

    if (!formula_solvable) LOG(INFO) << "LPSVM failed to solve this formula. Note that lpsvm_acc for this one will be garbage.";

    for (int i = 0; i < testing_x.size(); ++i) {
      if (lpsvm::predict(testing_x[i], m, d, training_x, training_y, a, b) == testing_y[i]) ++correct_count;
    }
    lpsvm_acc.push_back((double) correct_count / testing_x.size());

    correct_count = 0;
    std::vector<std::vector<int> > formula = ocat::train(training_x, training_y);

    for (int i = 0; i < testing_x.size(); ++i) {
      if (eval(true, formula, testing_x[i]) == (testing_y[i] == 1)) ++correct_count;
    }
    ocat_acc.push_back((double) correct_count / testing_x.size());

    LOG(INFO) << "OCAT CNF: " << to_string(true, formula);

    correct_count = 0;
    id3::Tree decision_tree(training_x, training_y);

    for (int i = 0; i < testing_x.size(); ++i) {
      if (decision_tree.predict(testing_x[i]) == (testing_y[i] == 1)) ++correct_count;
    }
    id3_acc.push_back((double) correct_count / testing_x.size());

    LOG(INFO) << "id3 DNF: " << to_string(false, decision_tree.getFormula(false));
    
    correct_count = 0;
    double weight[d], thresh;
    bool negated[d];
    winnow::train(weight, negated, thresh, training_x, training_y, d);

    for (int i = 0; i < testing_x.size(); ++i) {
      if (winnow::predict(testing_x[i], weight, negated, thresh, d) == testing_y[i]) ++correct_count;
    }
    winnow_acc.push_back((double) correct_count / testing_x.size());

  }

  for (int i = 0; i < lpsvm_acc.size(); ++i) {
    LOG(INFO) << "lpsvm: " << lpsvm_acc[i] << " ocat: " << ocat_acc[i] << " id3: " << id3_acc[i] << " winnow: " << winnow_acc[i];
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
  std::vector<int> y {-1, -1, 1, 1, -1, -1, 1, 1};

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
