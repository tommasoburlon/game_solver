#include <iostream>
#include <fstream>
#include <argparse.h>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <solver.h>

struct Args : public argparse::Args{
  std::string &name = arg<std::string>("name of the executable");
  bool &help = flag({"-h", "--help"}, "print help informations");
  std::string &dict  = kwarg<std::string>({"-d", "--dictionary"}, "[path to dictionary]");
  std::string &cache = kwarg<std::string>({"-c", "--cache"}, "[path to cache file]");
  int &word_size    = kwarg<int>({"-n", "--word-size"}, "size of the word to try", 5);
};

int main(int argc, const char *argv[]){
  Args args = argparse::parse<Args>(argc, argv);
  if(args.help){
    args.print();
    return 0;
  }

  std::vector<std::string> words;
  if(args.dict == "")
    return 0;
  std::ifstream file(args.dict);

  int ctr = 0;
  std::string parsed(args.word_size, '*');
  while(!file.eof()){
    std::string word;
    getline(file, word);
    bool format = true;
    int ctr = 0;
    for(size_t i = 0; i < word.size(); i++){
      if((word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= 'a' && word[i] <= 'z')){
        parsed[i] = toupper(word[i]);
        ctr++;
      }else if(word[i] != ' ' && word[i] != '\n' && word[i] != '\r'){
        format = false; break;
      }

      if(ctr > args.word_size) break;

    }
    if(format && ctr == args.word_size)
      words.push_back(parsed);
  }
  std::cout << "dictionary: " << args.dict << " loaded, words\n";
  std::cout << args.word_size << "-word: " << words.size() << std::endl;

  Solver solver(words);
  while(true){
    std::vector<std::string> solutions = solver.solutions();
    std::cout << "possible solutions: " << solutions.size() << std::endl;
    auto hints = solver.hints(20, 8);

    for(int i = 0; i < hints.size(); i++)
      std::cout << hints[i] << "      " << (i < solutions.size() ? solutions[i] : " ") << std::endl;

    std::string word, mask;
    std::cin >> word;
    std::cin >> mask;
    double score = solver.score(word);
    std::cout << "your score: " << score << ", accuracy: " << 100 * hints[0].score / score << "%" << std::endl;
    solver.turn(word, mask);
  }

  return 0;
}
