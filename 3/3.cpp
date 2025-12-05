#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::vector<int>> readMatrix(std::string_view filename) {
  std::vector<std::vector<int>> grid;
  std::ifstream file(filename.data());

  if (!file) {
    std::cerr << "Failed to open file: " << filename << '\n';
    return grid;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::vector<int> row;
    row.reserve(line.size());

    for (char c : line) {
      if (std::isdigit(static_cast<unsigned char>(c))) {
        row.push_back(c - '0');
      }
    }
    grid.push_back(std::move(row));
  }

  return grid;
}

long long maximiseBank(const std::vector<int> &bank) {
  int d{};
  std::size_t d_index{};

  for (std::size_t i{0}; i < bank.size() - 1; ++i) {
    if (bank[i] > d) {
      d = bank[i];
      d_index = i;
    }
  }

  int s{};

  for (std::size_t i{d_index + 1}; i < bank.size(); ++i) {
    if (bank[i] > s) {
      s = bank[i];
    }
  }

  return (10 * d + s);
}

std::vector<std::pair<int, std::size_t>>
sortedValuesWithIndices(const std::vector<int> &row) {
  std::vector<std::pair<int, std::size_t>> result{};
  const std::size_t len{row.size()};
  result.reserve(len);

  for (std::size_t i{0}; i < len; ++i) {
    result.emplace_back(row[i], i);
  }

  std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) {
    if (a.first != b.first)
      return a.first > b.first;
    return a.second < b.second;
  });

  return result;
}

long long maximiseBank2(const std::vector<std::pair<int, std::size_t>> &pairV,
                        int order) {

  const std::size_t len{pairV.size()};
  long long bankMax{0};

  int a{};
  int index_a{-1};

  while (order > 0) {
    for (std::size_t i{0}; i < len; ++i) {
      if ((pairV[i].second < len - order + 1) &&
          (static_cast<int>(pairV[i].second) > index_a)) {
        a = pairV[i].first;
        index_a = pairV[i].second;
        break;
      }
    }
    bankMax = 10 * bankMax + a;
    --order;
  }
  return bankMax;
}

int main() {
  // 7632362525633465259457646556334542742966514383538566582522246342353467256715675721556536637573534256
  // std::vector<int> test{8, 1, 8, 1, 8, 1, 9, 1, 1, 1, 1, 2, 1, 1, 1};
  // std::cout << maximiseBank2(sortedValuesWithIndices(test), 12) << '\n';

  std::vector<std::vector<int>> Matrix{readMatrix("input.txt")};

  long long totalJoltage1{};
  long long totalJoltage2{};

  for (const std::vector<int> &row : Matrix) {
    totalJoltage1 += maximiseBank(row);
    totalJoltage2 += maximiseBank2(sortedValuesWithIndices(row), 12);
  }
  std::cout << totalJoltage2 << '\n';
  return 0;
}
