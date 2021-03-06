#include <vector>
namespace lpsvm {
bool train(int m, int d, const std::vector<std::vector<int> > &x, const std::vector<int> &y, double a_store[], double& b_store);
int predict(const std::vector<int> &to_predict, int m, int d, const std::vector<std::vector<int> > &x, const std::vector<int> &y, double a[], double b);
std::vector<std::vector<int> > approx_formula(int m, int d, const std::vector<std::vector<int> > &x, const std::vector<int> &y, double a[]);
}