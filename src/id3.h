#include <vector>
#include <unordered_set>

class Tree {
  struct Node {
    int atom; // 0 indicates a leaf node
    union {
      bool label;
      Node *child[2];
    } payload;

    // Constructs a leaf
    Node(bool label);

    // Constructs a non-leaf node
    Node(int atom);

    ~Node();

    void getFormula(const bool &inCNF, std::vector<std::vector<int> > &formula_store, std::vector<int> &curBranch);

    private:
      Node(const Node&);
      Node& operator=(const Node&);
  };
  Node *root = nullptr;

  Tree(const Tree&);
  Tree& operator=(const Tree&);

  static Node* ID3(const std::vector<std::vector<int> *> &pos, const std::vector<std::vector<int> *> &neg, std::unordered_set<int> available);

  public:
    Tree(std::vector<std::vector<int> > &x, int y[]);

    ~Tree();

    bool predict(const std::vector<int> &sample);

    std::vector<std::vector<int> > getFormula(const bool &inCNF);

};

