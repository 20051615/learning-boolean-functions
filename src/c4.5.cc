// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"

struct Node {
  int atom; // 0 indicates a leaf node
  union {
    Node *child[2];
    bool label;
  } payload;

  // Constructs a leaf
  Node(bool label) {
    atom = 0;
    payload.label = label;
  }

  // Constructs a non-leaf node
  Node(int atom) {
    this->atom = atom;
  }

  ~Node() {
    if (atom) {
      delete payload.child[0];
      delete payload.child[1];
    }
  }
};

struct Tree {
  Node *root;

  // Note: only pass in dynamically allocated nodes
  Tree(Node *root) {
    this->root = root;
  }

  ~Tree() {
    delete root;
  }
};

int main() {
  Node* root = new Node(1);
  root->payload.child[0] = new Node(true);
  root->payload.child[1] = new Node(false);
  Tree tree(root);
  return EXIT_SUCCESS;
}
