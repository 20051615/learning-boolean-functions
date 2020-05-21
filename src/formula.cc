#include <vector>
#include <string>

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

bool DNF_clause_accepts(const std::vector<int> &clause, const std::vector<int> &input) {
  for (int i = 0; i < clause.size(); ++i) {
    if (clause[i] > 0) {
      if (!input[clause[i] - 1]) return false;
    } else {
      if (input[-clause[i] - 1]) return false;
    }
  }
  return true;
}

bool DNF_eval(const std::vector<std::vector<int> > &formula, const std::vector<int> &input) {
  for (int i = 0; i < formula.size(); ++i) {
    if (DNF_clause_accepts(formula[i], input)) return true;
  }
  return false;
}

// TODO: test DNF_eval. Or eval(false..)

bool eval(const bool &asCNF, const std::vector<std::vector<int> > &formula, const std::vector<int> &input) {
  if (formula.size() == 0) return true;
  if (formula.size() == 1 && formula[0].size() == 0) return false;

  if (asCNF)
    return CNF_eval(formula, input);
  else
    return DNF_eval(formula, input);
}

std::string to_CNF_string(const std::vector<std::vector<int> > &formula) {
  std::string formula_str = "(";
  for (int i = 0; i < formula.size(); ++i) {
    for (int j = 0; j < formula[i].size(); ++j) {
      formula_str += std::to_string(formula[i][j]) + (j != formula[i].size() - 1 ? " + " : "");
    }
    formula_str += (i != formula.size() - 1 ? ") * (" : ")");
  }
  return formula_str;
}

std::string to_DNF_string(const std::vector<std::vector<int> > &formula) {
  std::string formula_str;
  for (int i = 0; i < formula.size(); ++i) {
    for (int j = 0; j < formula[i].size(); ++j) {
      formula_str += std::to_string(formula[i][j]) + (j != formula[i].size() - 1 ? " * " : "");
    }
    formula_str += (i != formula.size() - 1 ? " + " : "");
  }
  return formula_str;
}

std::string to_string(const bool &fromCNF, const std::vector<std::vector<int> > &formula) {
  if (fromCNF)
    return to_CNF_string(formula);
  else
    return to_DNF_string(formula);
}

