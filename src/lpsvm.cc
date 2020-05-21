/*
 * This describes a linear-programming support vector machine.
 * Note that, since this is the first algorithm I implemented,
 * this file also serves as a template in terms of formatting, e.g.
 * where to use (0,1) and where to use (-1,1) for (F,T), and the like.
 */

#include "ortools/linear_solver/linear_solver.h"
#include <vector>

namespace ortools = operations_research;

namespace lpsvm {

// d is the number of boolean variables for the problem concerned
int kernel(const std::vector<int> &u, const std::vector<int> &v, int d) {
  int prod = 1;
  for (int i = 0; i < d; ++i) {
    prod *= (2 * u[i] * v[i] - u[i] - v[i] + 2);
  }
  return prod - 1;
}

// m is the size of the training set
// returns if the linear programming problem has been successfully solved
bool train(int m, int d, const std::vector<std::vector<int> > &x, const std::vector<int> &y, double a_store[], double& b_store) {
  // Create the linear solver with the GLOP backend.
  ortools::MPSolver solver("simple_lp_program", ortools::MPSolver::GLOP_LINEAR_PROGRAMMING);

  // Create the variables to solve for
  ortools::MPVariable* const b = solver.MakeNumVar(-ortools::MPSolver::infinity(), ortools::MPSolver::infinity(), "b");
  std::vector<ortools::MPVariable*> a;
  solver.MakeNumVarArray(m, 0.0, ortools::MPSolver::infinity(), "a", &a);

  // Create the constraints
  for (int i = 0; i < m; ++i) {
    ortools::MPConstraint* const ct = solver.MakeRowConstraint(1, ortools::MPSolver::infinity());
    for (int j = 0; j < m; ++j) {
      ct->SetCoefficient(a[j], y[i] * y[j] * kernel(x[j], x[i], d));
    }
    ct->SetCoefficient(b, y[i]);
  }

  // Create the objective function
  ortools::MPObjective* const objective = solver.MutableObjective();
  for (int i = 0; i < m; ++i) {
    objective->SetCoefficient(a[i], 1);
  }
  objective->SetMinimization();

  ortools::MPSolver::ResultStatus linear_program_result = solver.Solve();
  if (linear_program_result != ortools::MPSolver::ResultStatus::OPTIMAL) return false;

  b_store = b->solution_value();
  for (int i = 0; i < m; ++i) {
    a_store[i] = a[i]->solution_value();
  }

  return true;
}

int predict(const std::vector<int> &to_predict, int m, int d, const std::vector<std::vector<int> > &x, const std::vector<int> &y, double a[], double b) {
  double f = 0.0;
  for (int i = 0; i < m; ++i) {
    f += y[i] * a[i] * kernel(x[i], to_predict, d);
  }
  f += b;
  return (f >= 0.0) ? 1 : -1;
}

}