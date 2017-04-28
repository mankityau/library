#include <fstream>
#include <vector>
#include <iostream>
#include <thread>
#include <string>
#include "cpen333/thread/timer.h"

// will be used as a callback for our timer
class Singer {
 public:
  Singer(const std::vector<std::string>& lines) :
      lidx_{0}, lines_{lines} {}

  // override () operator
  void operator()() {
    std::cout << lines_[lidx_];
    std::cout.flush();
    ++lidx_;
    if (lidx_ == lines_.size()) {
      lidx_ = 0;
    }
  }

 private:
  size_t lidx_;
  std::vector<std::string> lines_;
};

int main() {

  std::vector<std::string> lines;

  // load data from file
  std::ifstream fin("./timer.dat");
  while (!fin.eof()) {
    std::string line;
    std::getline(fin, line);
    // substitute end-lines
    for (int i=0; i<line.size()-1; ++i) {
      if (line[i]== '\\' && line[i+1] == 'n') {
        line[i++] = ' ';  // place a space
        line[i] = '\n';
      }
    }
    lines.push_back(line);
  }

  // timer set for every half-second, call singer as a callback
  Singer singer(lines);
  cpen333::thread::timer<std::chrono::milliseconds> timer(std::chrono::milliseconds(450), singer);

  // run for about a minute, then stop
  timer.start();
  std::this_thread::sleep_for(std::chrono::seconds(30));
  timer.stop();

  std::cout << std::endl << "Done." << std::endl;

  return 0;
}