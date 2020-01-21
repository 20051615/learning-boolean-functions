// For ortools-style logging and such
#include "ortools/linear_solver/linear_solver.h"

struct Node {
  int atom; // 0 indicates a leaf node
  union Payload {
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

  // Should only be used for dynamically allocated nodes
  // Deallocates the entire tree rooted at this node
  // DO NOT USE FOR OBJECTS THAT LIVE ON THE STACK (delete this)
  void self_destruct() {
    if (atom) {
      payload.child[0]->self_destruct();
      payload.child[1]->self_destruct();
    }
    delete this;
  }
};

int main() {
  Node* root = new Node(1);
  root->payload.child[0] = new Node(true);
  root->payload.child[1] = new Node(false);
  root->self_destruct();
  return EXIT_SUCCESS;
}
