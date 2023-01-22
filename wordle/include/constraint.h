#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <string>
#include <vector>
#include <iostream>

#define ALPHABET ('Z' - 'A' + 1)

struct range{ int min, max; };

class constraint{
  std::vector<bool> char_table;
  range char_range[ALPHABET];
  int len;
public:
  constraint(int size = 5);
  constraint(std::string word, std::string mask);
  ~constraint();

  void print();

  bool refine();
  bool test(std::string);
  std::vector<std::string> filter(std::vector<std::string>);

  static constraint find_constraint(std::string base, std::string word);

  size_t size() const { return len; }

  bool operator==(const constraint &other) const {
    for(int i = 0; i < ALPHABET; i++){
      if(char_range[i].min != other.char_range[i].min) return false;
      if(char_range[i].max != other.char_range[i].max) return false;
      for(int j = 0; j < len; j++){
        if(char_table[i + j * ALPHABET] != other.char_table[i + j * ALPHABET])
          return false;
      }
    }
    return true;
  }

  void merge(const constraint& cstr);

  bool is_possible(int idx, int ch) const { return char_table[idx * ALPHABET + ch]; }
  int get_min(int ch) const { return char_range[ch].min; }
  int get_max(int ch) const { return char_range[ch].max; }

  void serialize(std::ostream&) const;
  static constraint deserialize(std::istream&);
};

template <> struct std::hash<constraint>{
  std::size_t operator()(const constraint& cstr) const {
    size_t ret = 0, exp, base_exp = 12231;
    exp = base_exp;
    for(int i = 0; i < ALPHABET; i++){
      ret += cstr.get_min(i) * exp;
      ret += cstr.get_max(i) * exp * base_exp;
      for(size_t j = 0; j < cstr.size(); j++)
        ret += cstr.is_possible(j, i) * exp * base_exp * base_exp;
      exp *= (base_exp * base_exp * base_exp);
    }
    return ret;
  }
};

#endif
