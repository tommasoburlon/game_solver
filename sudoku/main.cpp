#include <iostream>
#include "argparse.h"
#include <fstream>
#include <sstream>
#include <bitset>

class Sudoku{
  std::bitset<9> table[9][9];
public:
  Sudoku(std::string sudoku){
    int idx = 0;
    for(char ch : sudoku){
      if(ch == '\n')
        continue;
      int x, y;
      x = idx % 9;
      y = idx / 9;
      if(ch >= '1' && ch <= '9')
        add(x, y, ch - '0');
      else
        add_all(x, y);
      idx++;
    }
  }

  void print(bool all_possibility=true){
    std::string h_line, v_line;

    h_line = "+---------+---------+---------+";
    v_line = "|";

    std::cout << h_line << std::endl;
    for(int y = 0; y < 9; y++){
      std::cout << v_line;
      for(int x = 0; x < 9; x++){
        if(all_possibility){
          std::cout << "(";
          for(int value = 1; value <= 9; value++){
            if(is_possible(x, y, value))
              std::cout << value;
            else
              std::cout << " ";
          }
          std::cout << ")";
        }else{
          if(possibility(x, y) == 1)
            std::cout << " " << value(x, y) << " ";
          else
            std::cout << " * ";
        }

        if(x % 3 == 2)
          std::cout << v_line;
      }
      std::cout << std::endl;
      if(y % 3 == 2)
        std::cout << h_line << std::endl;
    }

  }

  Sudoku(){};
  ~Sudoku(){};

  int value(uint8_t x, uint8_t y){
    uint16_t value = (uint16_t)table[y][x].to_ulong(), ctr = 0;
    while(value){
      ctr++;
      value >>= 1;
    }
    return ctr;
  }

  void add(uint8_t x, uint8_t y, uint8_t value){
    table[y][x].set(value - 1);
  }

  void add_all(uint8_t x, uint8_t y){
    table[y][x] = std::bitset<9>{511};
  }

  void remove(uint8_t x, uint8_t y, uint8_t value){
    table[y][x].reset(value);
  }

  int possibility(uint8_t x, uint8_t y){
    return table[y][x].count();
  }

  bool is_possible(uint8_t x, uint8_t y, uint8_t value){
    return table[y][x][value - 1];
  }

  bool pruning(){
    bool result = false;
    for(int x = 0; x < 9; x++){
      for(int y = 0; y < 9; y++){
        std::bitset<9> c = constraint(x, y);
        if(!result && (table[y][x] & c) != table[y][x] )
          result = true;
        table[y][x] &= c;
      }
    }
    return result;
  }

  std::bitset<9> possible_mask(std::bitset<9*9> mask){
    std::bitset<9> response({0});
    for(int x = 0; x < 9; x++){
      for(int y = 0; y < 9; y++){
        if(mask[y * 9 + x])
          response |= table[y][x];
      }
    }
    return response;
  }

  std::bitset<9> presence_mask(std::bitset<9*9> mask){
    std::bitset<9> response({0});
    for(int x = 0; x < 9; x++){
      for(int y = 0; y < 9; y++){
        if(mask[y * 9 + x] && possibility(x, y) == 1)
          response |= table[y][x];
      }
    }
    return response;
  }

  std::bitset<9> possibility_mask(std::bitset<9*9> mask){
    std::bitset<9> response({0});
    for(int x = 0; x < 9; x++){
      for(int y = 0; y < 9; y++){
        if(mask[y * 9 + x])
          response |= table[y][x];
      }
    }
    return response;
  }

  std::bitset<9*9> make_mask_uniqueness(uint8_t x, uint8_t y){
    std::bitset<9*9> mask({0});
    int xb = (x / 3) * 3, yb = (y / 3) * 3;

    for(int i = 0; i < 9; i++){
      mask.set(x + 9 * i);
      mask.set(i + 9 * y);
      mask.set(xb + (i / 3) + 9 * (yb + i % 3));
    }
    mask.reset(x + 9 * y);
    return mask;
  }

  std::bitset<9*9> make_mask_row(uint8_t x, uint8_t y){
    std::bitset<9*9> mask({0});

    for(int i = 0; i < 9; i++)
      mask.set(i + 9 * y);
    return mask;
  }

  std::bitset<9*9> make_mask_col(uint8_t x, uint8_t y){
    std::bitset<9*9> mask({0});

    for(int i = 0; i < 9; i++)
      mask.set(x + 9 * i);
    return mask;
  }

  std::bitset<9*9> make_mask_block(uint8_t x, uint8_t y){
    std::bitset<9*9> mask({0});
    int xb = (x / 3) * 3, yb = (y / 3) * 3;

    for(int i = 0; i < 9; i++)
      mask.set(xb + (i / 3) + 9 * (yb + i % 3));
    return mask;
  }

  std::bitset<9> constraint(uint8_t x, uint8_t y){
    std::bitset<9> response({511}), temp;
    std::bitset<9*9> cell, mask, col, row, block;

    cell.set(x + y * 9);
    cell = ~cell;

    block = make_mask_block(x, y);
    temp = possibility_mask(block & cell);
    if(temp.count() == 8) return ~temp;

    row = make_mask_row(x, y);
    temp = possibility_mask(row & cell);
    if(temp.count() == 8) return ~temp;

    col = make_mask_col(x, y);
    temp = possibility_mask(col & cell);
    if(temp.count() == 8) return ~temp;

    temp = presence_mask((col | block | row) & cell);
    response &= (~temp);


    for(int i = 0; i < 3; i++){
      int xb = 3 * i, yb = 3 * i;

      if(xb != 3 * (x / 3)){
        mask = (make_mask_block(xb, y) & (~row));
        temp = ~possibility_mask(mask);
        response &= (~temp);
      }

      if(yb != 3 * (y / 3)){
        mask = (make_mask_block(x, yb) & (~col));
        temp = ~possibility_mask(mask);
        response &= (~temp);
      }

    }
    return response;
  }

  static bool solve(Sudoku *s, int x = 0, int y = 0){
    while(s->pruning()){}
    return false;
  }
};

struct MyArgs : public argparse::Args {
  std::string &name = arg<std::string>();
  bool &debug = flag({"-d", "--debug"});
  bool &verbose = flag({"-v", "--verbose"});
  std::string &file = kwarg<std::string>({"-f", "--file"});
  std::string &input = kwarg<std::string>({"-i", "--input"});
};

int main(int argc, char* argv[]){
  MyArgs args = argparse::parse<MyArgs>(argc, argv);
  Sudoku sudoku;

  if(args.file != ""){
    std::ifstream t(args.file);
    std::stringstream buffer;
    buffer << t.rdbuf();
    sudoku = Sudoku(buffer.str());
  }

  sudoku.print(args.debug);
  std::cout << std::endl;
  std::cout << std::endl;
  Sudoku::solve(&sudoku);
  sudoku.print(args.debug);
  return 0;
}
