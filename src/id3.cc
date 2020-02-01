// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"

#include <math.h>
#include <vector>
#include <unordered_set>
#include <limits>
#include <string>

#include "id3.h"

double entropy(int a, int b) {
  double entropy = 0;
  double sum = a + b;
  if (a) {
    double p = a / sum;
    entropy -= p * log2(p);
  }
  if (b) {
    double p = b / sum;
    entropy -= p * log2(p);
  }
  return entropy;
}

int best_atom(
              const std::vector<std::vector<int> *> &pos,
              const std::vector<std::vector<int> *> &neg,
              const std::unordered_set<int> &available
              ) {
  double min_weighted_entropy = std::numeric_limits<double>::max();
  int best_atom = 0;
  double total_size = pos.size() + neg.size();
  for (const int& atom: available) {
    double weighted_entropy = 0;
    int index = atom - 1;
    int pos_0 = 0, pos_1 = 0, neg_0 = 0, neg_1 = 0;
    for (int i = 0; i < pos.size(); ++i) {
      if ((*(pos[i]))[index]) ++pos_1;
      else ++pos_0;
    }
    for (int i = 0; i < neg.size(); ++i) {
      if ((*(neg[i]))[index]) ++neg_1;
      else ++neg_0;
    }
    weighted_entropy = ((pos_0 + neg_0) * entropy(pos_0, neg_0) + (pos_1 + neg_1) * entropy(pos_1, neg_1) ) / total_size;
    if (weighted_entropy < min_weighted_entropy) {
      min_weighted_entropy = weighted_entropy;
      best_atom = atom;
    }
  }
  return best_atom;
}
    
Tree::Node::Node(bool label): atom(0), payload{label} {}

Tree::Node::Node(int atom): atom(atom) {
  payload.child[0] = nullptr;
  payload.child[1] = nullptr;
}

Tree::Node::~Node() {
  if (atom) {
    delete payload.child[0];
    delete payload.child[1];
  }
}

void Tree::Node::getFormula(const bool &inCNF, std::vector<std::vector<int> > &formula_store, std::vector<int> &curBranch) {
  if (atom == 0) {
    if (inCNF && !payload.label) {
      for (int i = 0; i < curBranch.size(); ++i) {
        curBranch[i] = -curBranch[i];
      }
      formula_store.push_back(std::move(curBranch));
    } else if (!inCNF && payload.label) {
      formula_store.push_back(std::move(curBranch));
    }
  } else {
    std::vector<int> negBranch = curBranch;
    negBranch.push_back(-atom);
    payload.child[0]->getFormula(inCNF, formula_store, negBranch);
    curBranch.push_back(atom);
    payload.child[1]->getFormula(inCNF, formula_store, curBranch);
  }
}

Tree::Node* Tree::ID3(const std::vector<std::vector<int> *> &pos, const std::vector<std::vector<int> *> &neg, std::unordered_set<int> available) {
  if (neg.empty())
    return new Node(true);
  if (pos.empty())
    return new Node(false);
  if (available.empty())
    return new Node(pos.size() > neg.size() ? true : false);
  int atom = best_atom(pos, neg, available);
  Node* result = new Node(atom);
  available.erase(atom);

  std::vector<std::vector<int> *> pos_0, pos_1, neg_0, neg_1;
  int index = atom - 1;
  for (int i = 0; i < pos.size(); ++i) {
    if ((*(pos[i]))[index]) pos_1.push_back(pos[i]);
    else pos_0.push_back(pos[i]);
  }
  for (int i = 0; i < neg.size(); ++i) {
    if ((*(neg[i]))[index]) neg_1.push_back(neg[i]);
    else neg_0.push_back(neg[i]);
  }

  if (pos_0.empty() && neg_0.empty()) {
    result->payload.child[0] = new Node(pos.size() > neg.size() ? true : false);
  } else {
    result->payload.child[0] = ID3(pos_0, neg_0, available);
  }

  if (pos_1.empty() && neg_1.empty()) {
    result->payload.child[1] = new Node(pos.size() > neg.size() ? true : false);
  } else {
    result->payload.child[1] = ID3(pos_1, neg_1, available);
  }

  return result;
}

Tree::Tree(std::vector<std::vector<int> > &x, int y[]) {
  std::vector<std::vector<int> *> pos, neg;
  for (int i = 0; i < x.size(); ++i) {
    if (y[i] > 0) {
      pos.push_back(&(x[i]));
    } else {
      neg.push_back(&(x[i]));
    }
  }

  std::unordered_set<int> available;
  for (int atom = 1; atom <= x[0].size(); ++atom) {
    available.insert(atom);
  }

  root = ID3(pos, neg, std::move(available));
}

Tree::~Tree() {
  delete root;
}

bool Tree::predict(const std::vector<int> &sample) {
  Node* cur = root;
  while (cur->atom != 0) {
    int index = cur->atom - 1;
    cur = cur->payload.child[sample[index]];
  }
  return cur->payload.label;
}

std::vector<std::vector<int> > Tree::getFormula(const bool &inCNF) {
  std::vector<std::vector<int> > formula;
  std::vector<int> branch_store;
  root->getFormula(inCNF, formula, branch_store);
  return formula;
}

  // TODO: RUN VALGRIND TO TEST FOR LEAKS

