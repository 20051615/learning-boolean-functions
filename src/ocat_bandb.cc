/*
 * This describes the OCAT method, the version involving the full B&B search.
 */

#include <vector>
#include <string>
#include <limits>
#include <algorithm>

const int LIMIT_INIT = 5;
const int LIMIT_MULTIPLIER = 4;

namespace ocat {

bool cmp(const std::vector<int> &a, const std::vector<int> &b) {
  return a.size() < b.size();
}

// if an item is rejected, its size is returned; otherwise, a very large value is
int push_capped(std::vector<std::vector<int> > &store, int limit, const std::vector<int> &item) {
  if (store.size() < limit) {
    store.push_back(item);
    std::push_heap(store.begin(), store.end(), cmp);
    return std::numeric_limits<int>::max();
  } else {
    if (cmp(item, store[0])) {
      int toReturn = store[0].size();
      store.push_back(item);
      std::pop_heap(store.begin(), store.end(), cmp);
      store.pop_back();
      return toReturn;
    } else {
      return item.size();
    }
  }
}

/*
 * If optimality can be proven for the clause proposed in clause_store, 0 is returned.
 * Otherwise, a new best_known (size of the set in the terminal state) is returned.
 */
int propose_clause(const std::vector<std::vector<int> *> &pos, std::vector<std::vector<int> *> &neg, int limit, int best_known, std::vector<int> &clause_store) {
  int d = pos[0]->size();
  std::vector<int> empty_1d;

  std::vector<std::vector<int> > accepted_neg;
  for (int atom = -d; atom <= d; ++atom) {
    accepted_neg.push_back(empty_1d);

    if (atom < 0) {
      int index = -atom - 1;
      for (int i = 0; i < neg.size(); ++i) {
        if (!(*(neg[i]))[index]) accepted_neg[atom + d].push_back(i);
      }
    } else if (atom > 0) {
      int index = atom - 1;
      for (int i = 0; i < neg.size(); ++i) {
        if ((*(neg[i]))[index]) accepted_neg[atom + d].push_back(i);
      }
    }
  }

  int smallest_kicked = std::numeric_limits<int>::max();
  std::vector<std::vector<int> > a, b;
  a.push_back(empty_1d);
  std::vector<std::vector<int> > *next_level = &a;
  std::vector<std::vector<int> > *prev_level = &b;

  for (int level = 0; level < pos.size(); ++level) {
    // pos[level] is the positive sample that is to be accepted by the atom chosen at this level
    std::vector<std::vector<int> > *temp = next_level;
    next_level = prev_level;
    prev_level = temp;

    while (!prev_level->empty()) {
      /*
       * prev_level->back() is a selected search state from the {limit} number of search states from the previous level,
       * from which the next-level search states will be derived using atoms that accept pos[level]
       */
      for (int i = 0; i < d; ++i) {
        int atom = (i + 1) * (1 - 2 * (1 - (*(pos[level]))[i]));
        std::vector<int> next_state;
        std::set_union(
                       (prev_level->back()).begin(),
                       (prev_level->back()).end(),
                       accepted_neg[atom + d].begin(),
                       accepted_neg[atom + d].end(),
                       std::back_inserter(next_state)
                       );
        if (next_state.size() > best_known) continue;
        int kicked = push_capped(*next_level, limit, next_state);
        if (kicked < smallest_kicked) smallest_kicked = kicked;
      }
      prev_level->pop_back();
    }
  }

  std::vector<int> *best_solution;
  int best_size = std::numeric_limits<int>::max();
  for (int i = 0; i < next_level->size(); ++i) {
    if ((*next_level)[i].size() < best_size) {
      best_solution = &((*next_level)[i]);
      best_size = best_solution->size();
    }
  }

  if (best_solution->size() <= smallest_kicked) {
    // reconstructing the selected atoms from the terminal state
    std::vector<int> union_tester;
    for (int atom = -d; atom <= d; ++atom) {
      if (atom != 0) {
        std::set_union(
                       best_solution->begin(),
                       best_solution->end(),
                       accepted_neg[atom + d].begin(),
                       accepted_neg[atom + d].end(),
                       std::back_inserter(union_tester)
                       );
        if (union_tester.size() == best_solution->size())
          clause_store.push_back(atom);
        union_tester.clear();
      }
    }

    std::vector<std::vector<int> *> new_neg;
    for (int i = 0; i < best_solution->size(); ++i) {
      new_neg.push_back(neg[(*best_solution)[i]]);
    }
    neg = new_neg;
    return 0;
  } else {
    return best_solution->size();
  }
}

// input sanitization: code won't work if there are no negative examples. But of course.
std::vector<std::vector<int> > train(const bool &DNFmode, std::vector<std::vector<int> > &x, const std::vector<int> &y) {
  std::vector<std::vector<int> *> pos, neg;
  for (int i = 0; i < x.size(); ++i) {
    if ((y[i] > 0 && !DNFmode) || (y[i] <= 0 && DNFmode)) {
      pos.push_back(&(x[i]));
    } else {
      neg.push_back(&(x[i]));
    }
  }

  std::vector<std::vector<int> > result;

  if (neg.size() == 0) return result;
  if (pos.size() == 0) {
    std::vector<int> empty_stub;
    result.push_back(empty_stub);
    return result;
  }

  std::vector<int> clause;
  while (!neg.empty()) {
    int best_known = std::numeric_limits<int>::max();
    int limit = LIMIT_INIT;

    while (best_known != 0) {
      best_known = propose_clause(pos, neg, limit, best_known, clause);
      limit *= LIMIT_MULTIPLIER;
    }

    result.push_back(clause);
    clause.clear();
  }
  return result;
}

}