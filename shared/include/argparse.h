#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <initializer_list>
#include <cstring>

namespace argparse{

  template<class C>
  C parse(int argc, const char* argv[]){
    C parser;
    parser.parse(argc, argv);
    return parser;
  };

  typedef unsigned char byte;
  typedef void (*Parser)(const char*, byte*);
  typedef void (*Destructor)(byte*);

  template<class C> void str2type(const char* str, byte* ptr){
    /*C obj();
    *((C*)ptr) = obj;*/
  }

  template<class C> void free_object(byte* ptr){
    C* obj = (C*)ptr;
    delete obj;
  }

  enum ArgType{FLAG, KWARG, ARG};
  struct arg_data{
    byte* ptr;
    Parser parser;
    Destructor destructor;
    ArgType type;
    const char* description;
  };

  class Args{
    size_t positional_param;
    std::vector<arg_data> params;
    std::vector<const char*> unparsed;

    std::unordered_map<std::string, int> alias_to_index;
    std::vector<int> position_to_index;

    template<class C>
    C& add_arg(Parser parser, ArgType type, const char* description){
      C* arg = new C;
      arg_data data = {
        .ptr = (byte*)arg,
        .parser = parser,
        .destructor = free_object<C>,
        .type = type,
        .description = description
      };
      params.push_back(data);
      return *arg;
    }

  public:
    Args() : positional_param(0) {};
    ~Args();
    void print();
    void parse(int argc, const char* argv[]);
  protected:

    template<class C>
    C& kwarg(
      std::initializer_list<const char*> aliases,
      const char* description = "",
      C default_value = C(),
      Parser parser = str2type<C>
    ){
      for(const char* alias : aliases)
        alias_to_index.insert({alias, params.size()});
      C& ref = add_arg<C>(str2type<C>, KWARG, description);
      ref = default_value;
      return ref;
    }

    template<class C>
    C& arg(
      const char* description = "",
      C default_value = C(),
      Parser parser = str2type<C>
    ){
      position_to_index.push_back(params.size());
      C& ref = add_arg<C>(str2type<C>, ARG, description);
      ref = default_value;
      return ref;
    }

    bool& flag(
      std::initializer_list<const char*> aliases,
      const char* description = ""
    );
  };

};

#endif
