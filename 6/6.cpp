#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

std::vector<std::vector<int>> readInputNumbers(const std::string &filename) {
  std::vector<std::vector<int>> inputMatrix{};
  std::ifstream file(filename);
  std::string line{};

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::istringstream iss(line);
    std::vector<int> row{};
    int value{};

    while (iss >> value) {
      row.push_back(value);
    }

    if (!(row.empty())) {
      inputMatrix.push_back(std::move(row));
    }
  }
  return inputMatrix;
}
std::vector<std::string> readInput2(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }

  std::vector<std::string> inputVector;
  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty()) {
      inputVector.push_back(line);
    }
  }

  inputVector.pop_back();

  return inputVector;
}

std::vector<char> readInputOperations(const std::string &filename) {
  std::vector<char> inputOperations{};
  std::ifstream file(filename);

  std::string line{};
  std::string lastLine{};

  while (std::getline(file, line)) {
    if (!line.empty()) {
      lastLine = line;
    }
  }

  inputOperations.reserve(lastLine.size());

  for (char c : lastLine) {
    if (c == '+' || c == '*') {
      inputOperations.push_back(c);
    }
  }

  return inputOperations;
}

std::vector<std::vector<int>>
transpose(const std::vector<std::vector<int>> &matrix) {

  std::size_t m = matrix.size();
  std::size_t n = matrix[0].size();

  std::vector<std::vector<int>> matrix_transpose(n, std::vector<int>(m));

  for (std::size_t i = 0; i < m; ++i) {

    for (std::size_t j = 0; j < n; ++j) {
      matrix_transpose[j][i] = matrix[i][j];
    }
  }

  return matrix_transpose;
}

long long operate(const std::vector<int> &vec, char c) {
  switch (c) {
  case '+': {
    long long result = 0;
    for (int element : vec) {
      result += element;
    }
    return result;
  }

  case '*': {
    long long result = 1;
    for (int element : vec) {
      result *= element;
    }
    return result;
  }

  default:
    return -1;
  }
}

std::unordered_set<int> findBookMark(const std::vector<std::string> &v) {
  std::unordered_set<int> bookmark;

  if (v.empty()) {
    return bookmark;
  }

  int str_len = static_cast<int>(v[0].size());

  for (int str_pos = str_len - 1; str_pos >= 0; --str_pos) {
    bool allWhiteSpace = true;

    for (std::size_t i = 0; i < v.size(); ++i) {
      const std::string &str = v[i];
      if (str_pos >= static_cast<int>(str.size()) || str[str_pos] != ' ') {
        allWhiteSpace = false;
        break;
      }
    }

    if (allWhiteSpace) {
      bookmark.insert(str_pos);
    }
  }

  return bookmark;
}

long long operate2(const std::vector<std::string> &v,
                   const std::vector<char> &c,
                   const std::unordered_set<int> &bm) {

  const int str_len = static_cast<int>(v[0].size());
  long long globalResult = 0;

  std::vector<long long> localNums;
  localNums.reserve(str_len);

  int opr_indx = static_cast<int>(c.size()) - 1;
  char opr = c[opr_indx];

  for (int str_pos{str_len - 1}; str_pos >= 0; --str_pos) {

    if (bm.contains(str_pos)) {
      if (!localNums.empty()) {
        long long localResult;

        switch (opr) {
        case '+':
          localResult = 0;
          for (long long nums : localNums) {
            localResult += nums;
          }
          break;
        case '*':
          localResult = 1;
          for (long long nums : localNums) {
            localResult *= nums;
          }
          break;
        }

        globalResult += localResult;
        localNums.clear();
      }

      if (opr_indx > 0) {
        --opr_indx;
        opr = c[opr_indx];
      }

      continue;
    }

    long long localNum = 0;

    for (std::size_t i{0}; i < v.size(); ++i) {
      char ch = (str_pos < static_cast<int>(v[i].size())) ? v[i][str_pos] : ' ';

      int a{ch - '0'};
      if (a == -16) {
        continue;
      }
      localNum = 10 * localNum + a;
    }

    localNums.push_back(localNum);
  }

  if (!localNums.empty()) {
    long long localResult;

    switch (opr) {
    case '+':
      localResult = 0;
      for (long long nums : localNums) {
        localResult += nums;
      }
      break;
    case '*':
      localResult = 1;
      for (long long nums : localNums) {
        localResult *= nums;
      }
      break;
    }

    globalResult += localResult;
  }

  return globalResult;
}

int main() {
  std::string filename{"input.txt"};

  /* std::vector<std::vector<int>>
  matrix_t{transpose(readInputNumbers(filename))}; std::vector<char>
  operations{readInputOperations(filename)};

  std::size_t rows{matrix_t.size()};

  long long result{0};

  for (size_t i{0}; i < rows; ++i) {
    result += operate(matrix_t[i], operations[i]);
  }

  std::cout << "Part 1: " << result << '\n';
*/

  std::vector<std::string> inputVector{readInput2(filename)};
  std::vector<char> operations{readInputOperations(filename)};
  std::unordered_set<int> bm{findBookMark(inputVector)};

  std::cout << operate2(inputVector, operations, bm) << '\n';
  return 0;
}
