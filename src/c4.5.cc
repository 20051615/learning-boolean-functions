// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"

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
    Node(int atom): atom(atom) {}

    ~Node() {
      if (atom) {
        delete payload.child[0];
        delete payload.child[1];
      }
    }
  };
  Node *root;

  // Note: only pass in dynamically allocated nodes
  Tree(Node *root): root(root) {}

  public:
  ~Tree() {
    delete root;
  }

  static Tree test() {
    Node* root = new Node(1);
    root->payload.child[0] = new Node(true);
    root->payload.child[1] = new Node(false);
    Tree result(root);
    return result;
  }
};

int main() {
  Tree tree = Tree::test();
  return EXIT_SUCCESS;
}
