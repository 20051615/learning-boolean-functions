/*
 * This describes the OCAT method.
 * The polynomial heuristic for "step 2" is used. Branch-and-bound is NOT.
 * Simply see paper Figure 2 and Figure 3.
 * This is the first algorithm implemented that outputs a formula.
 * So this document is the template for how formulas are implemented as vectors of indices.
 */

// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"

#include <vector>
#include <string>

// each clause is a DISJUNCTION (a clause in CNF)
bool CNF_clause_accepts(const std::vector<int> &clause, const std::vector<int> &input) {
  for (int i = 0; i < clause.size(); ++i) {
    if (clause[i] > 0) {
      if (input[clause[i] - 1]) return true;
    } else {
      if (!input[-clause[i] - 1]) return true;
    }
  }
  return false;
}

bool CNF_eval(const std::vector<std::vector<int> > &formula, const std::vector<int> &input) {
  for (int i = 0; i < formula.size(); ++i) {
    if (!CNF_clause_accepts(formula[i], input)) return false;
  }
  return true;
}

// input sanitization: code won't work if there are no negative examples. But of course.
std::vector<std::vector<int> > train(int d, const std::vector<std::vector<int> > &x, int y[]) {
  std::vector<std::vector<int> > pos, neg;
  for (int i = 0; i < x.size(); ++i) {
    if (y[i] > 0) {
      pos.push_back(x[i]);
    } else {
      neg.push_back(x[i]);
    }
  }

  std::vector<std::vector<int> > result;
  while (!neg.empty()) {
LOG(INFO) << "neg size: " << neg.size();
    std::vector<std::vector<int> > remPos = pos;
    std::vector<int> clause;

    bool atom_allowed[2 * d + 1];
    for (int i = 0; i < 2 * d + 1; ++i) atom_allowed[i] = true;

    while (!remPos.empty()) {
LOG(INFO) << "remPos size: " << remPos.size();
      int atom_best = -d; // possible atoms range from -d to d, 0 excluded
      int pos_count_best = 0;
      int neg_count_best = 1;
      for (int atom = -d; atom <= d; ++atom) {
        int pos_count = 0, neg_count = 0;
        if (atom < 0 && atom_allowed[atom + d]) {
          int index = -atom - 1;
          for (int i = 0; i < neg.size(); ++i) {
            if (!neg[i][index]) {
              ++neg_count;
              if (neg_count_best == 0) goto skipatom;
            }
          }
          for (int i = 0; i < remPos.size(); ++i) {
            if (!remPos[i][index]) ++pos_count;
          }
        } else if (atom > 0 && atom_allowed[atom + d]) {
          int index = atom - 1;
          for (int i = 0; i < neg.size(); ++i) {
            if (neg[i][index]) {
              ++neg_count;
              if (neg_count_best == 0) goto skipatom;
            }
          }
          for (int i = 0; i < remPos.size(); ++i) {
            if (remPos[i][index]) ++pos_count;
          }
        } else {
          goto skipatom;
        }
LOG(INFO) << "atom: " << atom << " pos: " << pos_count << " neg: " << neg_count << " pos_best: " << pos_count_best << " neg_best: " << neg_count_best;

        if (neg_count_best == 0) {
          if (pos_count <= pos_count_best) goto skipatom;
        } else {
          if (neg_count > 0) {
            if (pos_count * neg_count_best <= pos_count_best * neg_count) goto skipatom;
          } else if (pos_count == 0) goto skipatom;
        }

LOG(INFO) << "new best";
        atom_best = atom;
        pos_count_best = pos_count;
        neg_count_best = neg_count;
skipatom:
        ;
      }

LOG(INFO) << "atom accepted: " << atom_best;
      clause.push_back(atom_best);
      atom_allowed[-atom_best + d] = false;
      atom_allowed[atom_best + d] = false;

      std::vector<std::vector<int> >::iterator it = remPos.begin();
      while (it != remPos.end()) {
        bool accepted = false;
        if (atom_best > 0) {
          if ((*it)[atom_best - 1]) accepted = true;
        } else {
          if (!(*it)[-atom_best - 1]) accepted = true;
        }
        if (accepted) {
          it = remPos.erase(it);
        } else ++it;
      }
    }
LOG(INFO) << "remPos emptied";

    std::vector<std::vector<int> >::iterator it = neg.begin();
    while (it != neg.end()) {
      if (!CNF_clause_accepts(clause, *it)) {
        it = neg.erase(it);
      } else ++it;
    }

    result.push_back(clause);
  }

  return result;
}

int main() {
  std::vector<std::vector<int> > x {
    {0, 0, 0},
    {0, 0, 1},
    {0, 1, 0},
    {0, 1, 1},
    {1, 0, 0},
    {1, 0, 1},
    {1, 1, 0},
    {1, 1, 1}
  };
  int y[] = {-1, -1, 1, -1, -1, 1, -1, -1};

  std::vector<std::vector<int> > formula = train(x[0].size(), x, y);
  std::string formula_str = "(";
  for (int i = 0; i < formula.size(); ++i) {
    for (int j = 0; j < formula[i].size(); ++j) {
      formula_str += std::to_string(formula[i][j]) + (j != formula[i].size() - 1 ? " + " : "");
    }
    formula_str += (i != formula.size() - 1 ? ") * (" : ")");
  }
  LOG(INFO) << "OCAT CNF: " << formula_str;

  for (int i = 0; i < x.size(); ++i) {
    LOG(INFO) << "Prediction for " << i << ":" << CNF_eval(formula, x[i]);
  }

  return EXIT_SUCCESS;
}
