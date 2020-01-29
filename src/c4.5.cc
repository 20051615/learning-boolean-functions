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

  public:
    Tree() {};

    ~Tree() {
      delete root;
    }

    static void test(Tree &tree) {
      Node* root = new Node(1);
      root->payload.child[0] = new Node(true);
      root->payload.child[1] = new Node(false);
      tree.root = root;
    }
};

int main() {
  Tree tree;
  Tree::test(tree);
  return EXIT_SUCCESS;
}
