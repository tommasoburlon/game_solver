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

void Args::parse(int argc, char* argv[]){
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

}

bool& Args::flag(
  std::initializer_list<const char*> aliases,
  const char* description
){
  for(const char* alias : aliases)
    alias_to_index.insert({alias, params.size()});
  bool &ref =  add_arg<bool>(str2type<bool>, FLAG);
  ref = false;
  return ref;
}
