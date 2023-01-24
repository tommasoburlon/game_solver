#ifndef SOLVER_H
#define SOLVER_H

#include <constraint.h>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <atomic>
#include <unordered_map>

struct hint_t{
  double score;
  bool is_solution;
  std::string word;

  auto operator<(const hint_t& node) const { return score > node.score; };
  friend std::ostream& operator<<(std::ostream& stream, const hint_t& hint){
    stream << hint.word << " [" << hint.score << "] ";
    if(hint.is_solution) stream << "is POSSIBLE ";
    return stream;
  }
};

class Solver{
  std::atomic<int> observed;
  std::vector<std::string> words, sols;

  std::priority_queue<hint_t> hints_job(int, int, int, std::unordered_set<std::string>);
public:
  Solver(std::vector<std::string> w) : words(w), sols(w) {};
  ~Solver(){};

  std::vector<hint_t> hints(int size = 1, int jobs = 1);
  double score(std::string word);
  void turn(std::string word, std::string pattern);

  std::vector<std::string> solutions(){ return sols; }
  void reset(){ sols = words; }
};

#endif
