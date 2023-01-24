#include "constraint.h"
#include <vector>
#include <iostream>
#include <cstring>


std::string find_pattern(std::string base, std::string word){
  std::string result = std::string(base.size(), '*');
  char char_counting[ALPHABET];
  memset(char_counting, 0, ALPHABET);

  for(int i = 0; i < base.size(); i++){
    if(base[i] == word[i]) result[i] = base[i];
    else char_counting[word[i] - 'A']++;
  }

  for(int i = 0; i < base.size(); i++){
    if(char_counting[base[i] - 'A'] > 0 && base[i] != word[i]){
      result[i] = tolower(base[i]);
      char_counting[base[i] - 'A']--;
    }
  }

  return result;
}
