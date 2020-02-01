#include <vector>
#include <string>

std::string to_string(const bool &fromCNF, const std::vector<std::vector<int> > &formula);
bool eval(const bool &asCNF, const std::vector<std::vector<int> > &formula, const std::vector<int> &input);

