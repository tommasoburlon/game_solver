#include "argparse.h"
#include <iostream>
#include <cstring>

using namespace argparse;


template<> void argparse::str2type<int>(const char* str, byte* ptr){
  int x = std::atoi(str);
  memcpy(ptr, &x, sizeof(int));
}

template<> void argparse::str2type<float>(const char* str, byte* ptr){
  float x = std::atoi(str);
  memcpy(ptr, &x, sizeof(float));
}

template<> void argparse::str2type<std::string>(const char* str, byte* ptr){
  std::string *in = (std::string*)ptr;
  (*in) = str;
}

Args::~Args(){
  for(arg_data data : params){
    data.destructor(data.ptr);
  }
}

void Args::parse(int argc, const char* argv[]){
  for(int i = 0; i < argc; i++){
    auto itr = alias_to_index.find(argv[i]);
    int idx;
    if(itr == alias_to_index.end()){
      if(positional_param >= position_to_index.size()){
        unparsed.push_back(argv[i]);
        continue;
      }
      idx = position_to_index[positional_param++];
    }else{
      idx = itr->second;
      i++;
    }
    argparse::arg_data data = params[idx];
    if(data.type == FLAG){
      i--;
      *data.ptr = true;
    }else{
      data.parser(argv[i], data.ptr);
    }
  }
}

void Args::print(){
  std::cout << "positional arguments:" << std::endl;
  for(arg_data arg : params){
    if(arg.type == ARG){
      std::cout << "\t[0] " << arg.description << std::endl;
    }
  }
  std::vector<std::vector<std::string>> aliases(params.size());
  for(auto itr : alias_to_index)
    aliases[itr.second].push_back(itr.first);

  std::cout << "\nkeyword argument: " << std::endl;
  for(size_t i = 0; i < aliases.size(); i++){
    if(params[i].type != KWARG)
      continue;
    std::cout << "\t";
    for(auto alias : aliases[i])
      std::cout << (alias == *aliases[i].begin() ? " " : ", ") << alias;
    std::cout << " " << params[i].description << std::endl;
  }

  std::cout << "\nflags: " << std::endl;
  for(size_t i = 0; i < aliases.size(); i++){
    if(params[i].type != FLAG)
      continue;
    std::cout << "\t";
    for(auto alias : aliases[i])
      std::cout <<  (alias == *aliases[i].begin() ? " " : ", ") << alias;
    std::cout << " " << params[i].description << std::endl;
  }
}

bool& Args::flag(
  std::initializer_list<const char*> aliases,
  const char* description
){
  for(const char* alias : aliases)
    alias_to_index.insert({alias, params.size()});
  bool &ref =  add_arg<bool>(str2type<bool>, FLAG, description);
  ref = false;
  return ref;
}
