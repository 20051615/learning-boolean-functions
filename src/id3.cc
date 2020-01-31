// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"
#include <math.h>
#include <vector>
#include <unordered_set>
#include <limits>

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

class Tree {
  struct Node {
    int atom; // 0 indicates a leaf node
    union {
      bool label;
      Node *child[2];
    } payload;

    // Constructs a leaf
    Node(bool label): atom(0), payload{label} {}

    // Constructs a non-leaf node
    Node(int atom): atom(atom) {
      payload.child[0] = nullptr;
      payload.child[1] = nullptr;
    }

    ~Node() {
      if (atom) {
        delete payload.child[0];
        delete payload.child[1];
      }
    }

    private:
      Node(const Node&);
      Node& operator=(const Node&);
  };
  Node *root = nullptr;

  Tree(const Tree&);
  Tree& operator=(const Tree&);

  static Node* ID3(const std::vector<std::vector<int> *> &pos, const std::vector<std::vector<int> *> &neg, std::unordered_set<int> available) {
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

  public:
    Tree(std::vector<std::vector<int> > &x, int y[]) {
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

    ~Tree() {
      delete root;
    }

    bool predict(const std::vector<int> &sample) {
      Node* cur = root;
      while (cur->atom != 0) {
        int index = cur->atom - 1;
        cur = cur->payload.child[sample[index]];
      }
      return cur->payload.label;
    }

};

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
  int y[] = {-1, -1, 1, -1, -1, 1, -1, -1, 1, -1, -1, 1, -1, -1, 1, -1};

  Tree decision_tree(x, y);

  for (const std::vector<int> &sample : x) {
    LOG(INFO) << decision_tree.predict(sample);
  }

  // TODO: RUN VALGRIND TO TEST FOR LEAKS
  // TODO: EXTRACT DNF FROM TREE
  return EXIT_SUCCESS;
}
