#include <vector>
#include <string>

std::string to_string(const bool &fromCNF, const std::vector<std::vector<int> > &formula);
bool eval(const bool &asCNF, const std::vector<std::vector<int> > &formula, const std::vector<int> &input);

// To represent simply T or F: T is represented by an empty 2D int vector, that is, {}; F is represented by {{}}.