#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::vector<uint8_t>> readBinaryMatrix(std::string_view filename) {
  std::ifstream file(static_cast<std::string>(filename));
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + std::string(filename));
  }

  std::vector<std::vector<uint8_t>> matrix{};
  std::string line{};

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    std::vector<uint8_t> row{};
    row.reserve(line.size());

    for (char c : line) {
      if (c == '.')
        row.push_back(0);
      else if (c == '@')
        row.push_back(1);
    }

    matrix.push_back(std::move(row));
  }

  if (matrix.empty())
    return matrix;

  std::size_t rows = matrix.size();
  std::size_t cols = matrix[0].size();

  std::vector<std::vector<uint8_t>> padded(rows + 2,
                                           std::vector<uint8_t>(cols + 2, 0));

  for (std::size_t i = 0; i < rows; ++i) {
    for (std::size_t j = 0; j < cols; ++j) {
      padded[i + 1][j + 1] = matrix[i][j];
    }
  }

  return padded;
}

long long countAccessible(const std::vector<std::vector<uint8_t>> &matrix) {

  std::size_t rows = matrix.size();
  std::size_t cols = matrix[0].size();

  long long count{0};

  for (std::size_t i{1}; i < rows - 1; ++i) {
    for (std::size_t j{1}; j < cols - 1; ++j) {

      if (!matrix[i][j]) {
        std::cout << '.';
        continue;
      }

      int adj{0};

      adj += matrix[i + 1][j + 1];
      adj += matrix[i + 1][j];
      adj += matrix[i + 1][j - 1];
      adj += matrix[i][j - 1];
      adj += matrix[i - 1][j - 1];
      adj += matrix[i - 1][j];
      adj += matrix[i - 1][j + 1];
      adj += matrix[i][j + 1];

      if (adj < 4) {
        std::cout << 'x';
        ++count;
      } else {
        std::cout << '@';
      }
    }
    std::cout << '\n';
  }

  std::cout << '\n';
  return count;
}

long long removeAccessible(std::vector<std::vector<uint8_t>> &matrix) {

  std::size_t rows = matrix.size();
  std::size_t cols = matrix[0].size();

  long long removed{0};
  long long count{0};

  do {

    count = 0;

    for (std::size_t i{1}; i < rows - 1; ++i) {
      for (std::size_t j{1}; j < cols - 1; ++j) {

        if (!matrix[i][j]) {
          std::cout << '.';
          continue;
        }

        int adj{0};

        adj += matrix[i + 1][j + 1];
        adj += matrix[i + 1][j];
        adj += matrix[i + 1][j - 1];
        adj += matrix[i][j - 1];
        adj += matrix[i - 1][j - 1];
        adj += matrix[i - 1][j];
        adj += matrix[i - 1][j + 1];
        adj += matrix[i][j + 1];

        if (adj < 4) {
          std::cout << 'x';
          matrix[i][j] = 0;
          ++count;
        } else {
          std::cout << '@';
        }
      }
      std::cout << '\n';
    }
    if (count != 0) {
      if (count > 1) {
        std::cout << "\nRemove " << count << " rolls of paper: \n";
      } else {
        std::cout << "\nRemove " << count << " roll of paper: \n";
      }
    }
    removed += count;
  } while (count > 0);

  return removed;
}

int main() {
  std::string_view fileName{"input.txt"};
  std::vector<std::vector<uint8_t>> matrix{readBinaryMatrix(fileName)};

  std::cout << "Part 1) " << countAccessible(matrix)
            << " rolls of paper can be accessed by a forklift.\n";
  std::cout << "Part 2) \n"
            << removeAccessible(matrix) << " rolls of paper can be removed.\n";

  return 0;
}
