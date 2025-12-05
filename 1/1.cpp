#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> readInput(const std::string &fileName) {
  std::ifstream file(fileName);

  std::vector<std::string> lines;

  for (std::string line; std::getline(file, line);) {
    lines.push_back(line);
  }

  std::cout << "Loaded " << lines.size() << " lines\n";
  return lines;
}

int turnKey(int current, const std::string &rotation) {
  char dir{rotation[0]};
  int move{0};
  for (std::size_t i = 1; i < rotation.size(); ++i) {
    char c{rotation[i]};
    move = move * 10 + (c - '0');
  }

  int delta{(dir == 'L') ? -move : move};
  int next{current + delta};
  next = ((next % 100) + 100) % 100;

  return next;
}

int zeroCrossed(int current, const std::string &rotation) {
  char dir{rotation[0]};
  int move{0};
  for (std::size_t i = 1; i < rotation.size(); ++i) {
    move = move * 10 + (rotation[i] - '0');
  }

  if (dir == 'R') {
    return (current + move) / 100;
  } else { // 'L'
    if (current > 0) {
      return (move + 100 - current) / 100;
    } else {
      return move / 100;
    }
  }
}

int main() {
  std::vector<std::string> lines{readInput("input.txt")};

  int key{50};
  int zeros{0};
  int crossings{0};

  for (const std::string &s : lines) {
    crossings += zeroCrossed(key, s);
    key = turnKey(key, s);
    if (key == 0) {
      ++zeros;
    }
  }

  std::cout << "Part 1:\n" << zeros << "\nPart2\n" << crossings << '\n';

  return 0;
}
