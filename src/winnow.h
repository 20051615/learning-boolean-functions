#include <vector>

namespace winnow {
void train(double weight[], bool negated[], double &thresh, const std::vector<std::vector<int> > &x, const std::vector<int> &y, int d);
int predict(const std::vector<int> &to_predict, double weight[], bool negated[], double &thresh, int d);
}