#include <vector>
void train(int m, int d, const std::vector<std::vector<int> > &x, int y[], double a_store[], double* b_store);
int predict(const std::vector<int> &to_predict, int m, int d, const std::vector<std::vector<int> > &x, int y[], double a[], double b);
