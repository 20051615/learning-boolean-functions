#include <vector>

namespace winnow {
void train(double weight[], const std::vector<std::vector<int> > &x, int y[], int d);
int predict(const std::vector<int> &to_predict, double weight[], int d);
}