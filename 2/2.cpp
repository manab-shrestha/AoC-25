#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<long long, long long>>
readRanges(const std::string &fileName) {
  std::ifstream file(fileName);
  std::vector<std::pair<long long, long long>> ranges{};

  std::string token;
  while (std::getline(file, token, ',')) {
    if (token.empty())
      continue;

    token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());

    size_t dash = token.find('-');

    long long left = std::stoll(token.substr(0, dash));
    long long right = std::stoll(token.substr(dash + 1));

    ranges.emplace_back(left, right);
  }

  return ranges;
}

bool isDouble(const long long &num) {
  std::string s{std::to_string(num)};
  if (std::ssize(s) % 2 != 0)
    return false;
  std::size_t half = s.size() / 2;
  return s.substr(0, half) == s.substr(half);
}

bool isRepeated(const long long &num) {
  std::string s = std::to_string(num);
  int n = static_cast<int>(s.size());

  for (int k = 2; k <= n; ++k) {
    if (n % k != 0)
      continue;

    int blockLen = n / k;
    bool ok = true;

    for (int i = blockLen; i < n; i += blockLen) {
      if (s.compare(0, blockLen, s, i, blockLen) != 0) {
        ok = false;
        break;
      }
    }

    if (ok)
      return true;
  }

  return false;
}

long long sumDouble(const std::vector<std::pair<long long, long long>> &arr) {
  long long sum{};
  for (const std::pair<long long, long long> &pair : arr) {
    for (long long num{pair.first}; num <= pair.second; ++num) {
      if (isDouble(num))
        sum += num;
    }
  }
  return sum;
}
long long sumRepeated(const std::vector<std::pair<long long, long long>> &arr) {
  long long sum{};
  for (const std::pair<long long, long long> &pair : arr) {
    for (long long num{pair.first}; num <= pair.second; ++num) {
      if (isRepeated(num))
        sum += num;
    }
  }
  return sum;
}

int main() {
  std::cout << sumDouble(readRanges("input.txt")) << '\n';
  std::cout << sumRepeated(readRanges("input.txt")) << '\n';
  return 0;
}
