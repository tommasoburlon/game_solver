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
  double value;
  bool is_possible;
  std::string word;

  auto operator<(const hint_t& node) const { return value > node.value; };
  friend std::ostream& operator<<(std::ostream& stream, const hint_t& hint){
    stream << hint.word << " [" << hint.value << "] ";
    if(hint.is_possible) stream << "is POSSIBLE ";
    return stream;
  }

  void serialize(std::ostream&) const;
  static hint_t deserialize(std::istream&);
};

class Solver{
  std::atomic<int> observed;
  std::vector<std::string> words, possible_solutions;

  std::priority_queue<hint_t> hints_job(int, int, int, std::unordered_set<std::string>);

  constraint cstr;
  std::unordered_map<constraint, std::vector<hint_t>> cache;

public:
  Solver(std::vector<std::string> w) : words(w), possible_solutions(w), cstr(w[0].size()) {};
  ~Solver(){};

  std::vector<hint_t> hints(int size = 1, int jobs = 1);
  double score(std::string word);
  void turn(constraint cstr);

  constraint get_constraint(){ return cstr; };
  std::vector<std::string> solutions(){ return possible_solutions; }
  void reset(){
    cstr = constraint(words[0].size());
    possible_solutions = words;
  }

  void read_cache(std::istream&);
  void write_cache(std::ostream&);
};

#endif
