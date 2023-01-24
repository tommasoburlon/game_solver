#include <solver.h>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <future>
#include <chrono>

/*void hint_t::serialize(std::ostream& stream) const {
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
}*/

double Solver::score(std::string word){
  std::unordered_map<std::string, int> table;
  int len = 0;
  for(std::string& other : sols){
    std::string pattern = find_pattern(word, other);
    auto res = table.insert({pattern, 0});
    len += res.second;
    res.first->second++;
  }
  return (double)sols.size() / len;
}

std::priority_queue<hint_t> Solver::hints_job(int start, int end, int size, std::unordered_set<std::string> table){
  double PRESENCE_VALUE = 1.0 / (double)solutions().size();
  std::priority_queue<hint_t> queue, res;

  for(int i = start; i < end; i++){
    std::string word = words[i];
    double value = score(word);
    bool is_possible = (table.find(word) != table.end());
    hint_t hint = {
      .score = value - (is_possible ? PRESENCE_VALUE : 0),
      .is_solution = is_possible,
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
  std::vector<std::future<std::priority_queue<hint_t>>> queues;
  std::priority_queue<hint_t> master_queue;
  std::unordered_set<std::string> table;

  for(auto word : sols)
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

  return result;
}

void Solver::turn(std::string word, std::string pattern){
  std::vector<std::string> new_sols;
  for(int i = 0; i < sols.size(); i++){
    std::string new_pattern = find_pattern(word, sols[i]);
    if(new_pattern == pattern)
      new_sols.push_back(sols[i]);
  }
  sols = new_sols;
}
