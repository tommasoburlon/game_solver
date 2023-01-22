#include "constraint.h"
#include <vector>
#include <iostream>
#include <cstring>


constraint::~constraint(){
}

constraint::constraint(int size){
  len = size;
  char_table = std::vector<bool>(len * ALPHABET, true);

  for(int i = 0; i < ALPHABET; i++)
    char_range[i] = {.min=0, .max=size};
}

constraint::constraint(std::string word, std::string mask){
  len = word.size();
  char_table = std::vector<bool>(len * ALPHABET, true);

  for(int i = 0; i < ALPHABET; i++)
    char_range[i] = {.min=0, .max=(int)word.size()};

  for(size_t i = 0; i < mask.size(); i++){
    int key = word[i] - 'A';
    if(mask[i] >= 'a' && mask[i] <= 'z'){
      char_range[key].min++;
      char_range[key].max = std::max(char_range[key].min, char_range[key].max);
      char_table[key + i * ALPHABET] = false;
    }else if(mask[i] >= 'A' && mask[i] <= 'Z'){
      char_range[key].min++;
      char_range[key].max = std::max(char_range[key].min, char_range[key].max);
      for(int j = 0; j < ALPHABET; j++) char_table[j + i * ALPHABET] = false;
      char_table[key + i * ALPHABET] = true;
    }else{
      char_table[key + i * ALPHABET] = false;
      char_range[key].max = char_range[key].min;
    }
  }
  refine();
}

bool constraint::refine(){
  int not_found = 5;
  bool is_changed = false;
  for(int i = 0; i < ALPHABET; i++)
    not_found -= char_range[i].min;

  for(int i = 0; i < ALPHABET; i++){
    if(char_range[i].max != char_range[i].min + not_found) is_changed = true;
    char_range[i].max = char_range[i].min + not_found;
  }

  for(int i = 0; i < ALPHABET; i++){
    bool is_zero = char_range[i].max == 0;
    for(int j = 0; j < len; j++){
      if(is_zero){
        if(char_table[i + j * ALPHABET]) is_changed = true;
        char_table[i + j * ALPHABET] = false;
      }
    }
  }

  if(is_changed) return refine();
  return is_changed;
}

bool constraint::test(std::string word){
  char word_counter[ALPHABET];
  memset(word_counter, 0, ALPHABET);

  for(size_t i = 0; i < word.size(); i++){
    int key = word[i] - 'A';
    word_counter[key]++;
    if(!char_table[key + i * ALPHABET]) return false;
  }

  for(int i = 0; i < ALPHABET; i++){
    if(word_counter[i] < char_range[i].min || word_counter[i] > char_range[i].max)
      return false;
  }

  return true;
}

std::vector<std::string> constraint::filter(std::vector<std::string> in){
  std::vector<std::string> result;
  for(std::string word : in)
    if(test(word))
      result.push_back(word);
  return result;
}

constraint constraint::find_constraint(std::string base, std::string word){
  constraint result(base.size());
  char word_counter[ALPHABET];
  memset(word_counter, 0, ALPHABET);

  for(size_t i = 0; i < base.size(); i++){
    int key = base[i] - 'A';
    word_counter[word[i] - 'A']++;
    if(base[i] == word[i]){
      result.char_range[key].min++;
      for(int j = 0; j < ALPHABET; j++)
        result.char_table[j + i * ALPHABET] = false;
      result.char_table[key + i * ALPHABET] = true;
    }
  }

  for(int i = 0; i < ALPHABET; i++) word_counter[i] -= result.char_range[i].min;

  for(size_t i = 0; i < base.size(); i++){
    if(base[i] != word[i]){
      int key = base[i] - 'A';
      result.char_table[key + i * ALPHABET] = false;
      if(word_counter[i] > 0){
        result.char_range[key].min++;
      }else{
        result.char_range[key].max = result.char_range[key].min;
      }
    }
  }


  result.refine();
  return result;
}

void constraint::merge(const constraint& cstr){
  for(int i = 0; i < ALPHABET; i++){
    char_range[i].min = std::max(char_range[i].min, cstr.char_range[i].min);
    char_range[i].max = std::min(char_range[i].max, cstr.char_range[i].max);
    for(int j = 0; j < len; j++){
      bool val1, val2;
      val1 = char_table[j * ALPHABET + i];
      val2 = cstr.char_table[j * ALPHABET + i];
      char_table[j * ALPHABET + i] = val1 & val2;
    }
  }
  refine();
}

void constraint::print(){
  for(int i = 0; i < ALPHABET; i++)
    std::cout << char_range[i].min << "," << char_range[i].max << "|";
  std::cout << std::endl;

  for(int j = 0; j < len; j++){
    for(int i = 0; i < ALPHABET; i++){
      std::cout << " ";
      std::cout << (char)(char_table[ALPHABET * j + i] ? (i + 'a') : ' ');
      std::cout << " |";
    }
    std::cout << std::endl;
  }
}

void constraint::serialize(std::ostream& stream) const {
  stream.write((char*)&len, sizeof(len));
  stream.write((char*)char_range, ALPHABET * sizeof(range));

  uint8_t data = 0;
  for(int i = 0; i < 1 + (len * ALPHABET) / sizeof(uint8_t); i++){
    data = 0;
    for(int j = 0; j < sizeof(uint8_t); j++){
      int key = i * sizeof(uint8_t) + j;
      if(key >= len * ALPHABET) break;
      data <<= 1;
      data |= char_table[key];
    }
    stream.write((char*)&data, sizeof(uint8_t));
  }
}

constraint constraint::deserialize(std::istream& stream){
  int len;
  stream.read((char*)&len, sizeof(len));

  constraint result(len);
  stream.read((char*)result.char_range, ALPHABET * sizeof(range));

  uint8_t data = 0;
  for(int i = 0; i < 1 + (len * ALPHABET) / sizeof(uint8_t); i++){
    stream.read((char*)&data, sizeof(uint8_t));
    for(int j = 0; j < sizeof(uint8_t); j++){
      int key = i * sizeof(uint8_t) + j;
      if(key >= len * ALPHABET) break;
      result.char_table[key] = data & 1;
      data >>= 1;
    }
  }

  return result;
}
