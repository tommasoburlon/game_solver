#include <solver.h>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <future>
#include <chrono>

void hint_t::serialize(std::ostream& stream) const {
  stream.write((char*)&value, sizeof(value));
  stream.write((char*)&is_possible, sizeof(is_possible));

  int len = word.size();
  stream.write((char*)&len, sizeof(len));
  stream.write((char*)word.c_str(), len);
}

hint_t hint_t::deserialize(std::istream& stream){
  hint_t ret;
  stream.read((char*)&ret.value, sizeof(value));
  stream.read((char*)&ret.is_possible, sizeof(is_possible));

  int len;
  stream.read((char*)&len, sizeof(len));

  ret.word = std::string(len, '*');
  for(int i = 0; i < len; i++)
    stream.read((char*)&ret.word[i], 1);
  return ret;
}

double Solver::score(std::string word){
  std::unordered_map<constraint, int> table;
  for(auto other : possible_solutions){
    constraint cstr = constraint::find_constraint(word, other);
    auto res = table.insert({cstr, 0});
    res.first->second++;
  }

  int ctr = 0, len = 0;
  for(auto itr : table){
    ctr += itr.second;
    len++;
  }
  return (double)ctr / len;
}

std::priority_queue<hint_t> Solver::hints_job(int start, int end, int size, std::unordered_set<std::string> table){
  double PRESENCE_VALUE = 1.0 / (double)solutions().size();
  std::priority_queue<hint_t> queue, res;

  for(int i = start; i < end; i++){
    std::string word = words[i];
    double value = score(word);
    bool is_possible = (table.find(word) != table.end());
    hint_t hint = {
      .value = value - (is_possible ? PRESENCE_VALUE : 0),
      .is_possible = is_possible,
      .word = word
    };

    observed++;
    queue.push(hint);
  }

  for(int i = 0; i < size; i++){
    if(queue.empty())
      break;
    res.push(queue.top());
    queue.pop();
  }
  return res;
}

void draw_bar(float percentage, int bar_size, bool newline = true){
  std::cout << "|";
  for(int i = 0; i < bar_size; i++)
    std::cout << ((float)i/bar_size > percentage ? ' ' : 'O');
  std::cout << "| " << 100 * percentage <<"%      \r" << std::flush;
  if(newline)
    std::cout << std::endl;
}

std::vector<hint_t> Solver::hints(int size, int jobs){
  auto itr = cache.find(cstr);
  if(itr != cache.end())
    return itr->second;
  std::vector<std::future<std::priority_queue<hint_t>>> queues;
  std::priority_queue<hint_t> master_queue;
  std::unordered_set<std::string> table;
  for(auto word : possible_solutions)
    table.insert(word);

  observed = 0;
  for(int i = 0; i < jobs; i++){
    int start, end;
    start = (i * words.size()) / jobs;
    end   = ((i + 1) * words.size()) / jobs;
    queues.push_back(std::async(&Solver::hints_job, this, start, end, size, table));
  }

  for(int i = 0; i < jobs; i++){
    std::chrono::milliseconds span(10);
    while (queues[i].wait_for(span) == std::future_status::timeout)
      draw_bar((float)observed / words.size(), 150, false);
    auto q = queues[i].get();
    while(!q.empty()){
      master_queue.push(q.top());
      q.pop();
    }
  }
  draw_bar(1, 150, true);

  std::vector<hint_t> result;

  for(int i = 0; i < size; i++){
    if(master_queue.empty()) break;
    result.push_back(master_queue.top());
    master_queue.pop();
  }

  cache.insert({cstr, result});
  return result;
}

void Solver::turn(constraint _cstr){
  possible_solutions = _cstr.filter(possible_solutions);
  cstr.merge(_cstr);
}


void Solver::read_cache(std::istream& stream){
  int len, word_size, num_word;
  stream.read((char*)&len, sizeof(len));
  stream.read((char*)&word_size, sizeof(word_size));
  for(int i = 0; i < len; i++){
    constraint idx = constraint::deserialize(stream);
    std::vector<hint_t> hint_arr;
    stream.read((char*)&num_word, sizeof(num_word));
    for(int j = 0; j < num_word; j++){
      hint_t hint = hint_t::deserialize(stream);
      hint_arr.push_back(hint);
    }

    idx.print();

    if(possible_solutions.size() > 100)
      cache.insert({idx, hint_arr});
  }
}

void Solver::write_cache(std::ostream& stream){
  int len = cache.size(), word_size = cstr.size();
  stream.write((char*)&len, sizeof(len));
  stream.write((char*)&word_size, sizeof(word_size));
  for(auto itr : cache){
    itr.first.serialize(stream);
    len = itr.second.size();
    stream.write((char*)&len, sizeof(len));
    for(auto hint : itr.second){
      hint.serialize(stream);
    }
  }
}
