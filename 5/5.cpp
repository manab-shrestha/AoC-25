#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

std::vector<std::pair<long long, long long>> getFresh(std::string &filename) {
  std::vector<std::pair<long long, long long>> fresh{};
  std::ifstream file(filename);
  std::string line{};

  while (std::getline(file, line)) {
    if (line.empty()) {
      break;
    }
    long long a{};
    long long b{};
    char dash{'-'};

    std::istringstream iss(line);

    if (!(iss >> a >> dash >> b) || dash != '-') {
      throw std::runtime_error("Bad input format: " + line);
    }

    if (a > b)
      std::swap(a, b);

    fresh.emplace_back(a, b);
  }

  return fresh;
}

std::vector<long long> getAvaliable(std::string &filename) {
  std::vector<long long> avaliable{};
  std::ifstream file(filename);
  std::string line{};
  bool pastBlank{false};

  while (std::getline(file, line)) {
    if (!pastBlank) {
      if (line.empty())
        pastBlank = true;
      continue;
    }

    long long num{};
    std::istringstream iss(line);

    if (!(iss >> num)) {
      throw std::runtime_error("Invalid number: " + line);
    }

    avaliable.push_back(num);
  }

  return avaliable;
}

int countAvaliableFresh(std::string &filename) {

  std::vector<std::pair<long long, long long>> fresh{getFresh(filename)};
  std::vector<long long> avaliable = getAvaliable(filename);

  int count{};

  for (long long n : avaliable) {
    bool isFresh{false};

    for (const std::pair<long long, long long> &p : fresh) {
      if (n >= p.first && n <= p.second) {
        ++count;
        // std::cout << "Fresh \n";
        isFresh = true;
        break;
      }
    }
    if (!isFresh) {
      // std::cout << "Spoiled \n";
      continue;
    }
  }

  return count;
}

/*
template <typename T>
std::size_t lenRemoveDuplicates(const std::vector<T> &vec) {
  std::unordered_set<T> seen{};
  std::vector<T> result{};
  result.reserve(vec.size());

  for (const T &v : vec) {
    if (seen.insert(v).second) {
      result.push_back(v);
    }
  }

  return std::ssize(result);
}

std::vector<long long> rangeFresh(std::string &filename) {

  std::vector<std::pair<long long, long long>> fresh{getFresh(filename)};
  std::vector<long long> rangeWithDuplicates{};

  std::size_t len{};

  for (const std::pair<long long, long long> &p : fresh) {
    len += (1 + p.second - p.first);
  }

  rangeWithDuplicates.reserve(len);

  for (const std::pair<long long, long long> &p : fresh) {
    for (long long i{p.first}; i <= p.second; ++i)
      rangeWithDuplicates.push_back(i);
  }

  return rangeWithDuplicates;
}*/

void mergeOverlap(std::vector<std::pair<long long, long long>> &vec) {

  std::size_t len{std::size(vec)};
  bool merging{};

  do {
    merging = false;

    for (size_t i{0}; i < len; ++i) {
      if (vec[i].first == 0 && vec[i].second == 0)
        continue;

      for (size_t j{0}; (j < len) && (j != i); ++j) {
        if (!(vec[j].first && vec[j].second))
          continue;

        if ((vec[i].first <= vec[j].second) &&
            (vec[j].first <= vec[i].second)) {

          vec[i].first = std::min(vec[i].first, vec[j].first);
          vec[i].second = std::max(vec[i].second, vec[j].second);

          // mark j as dead
          vec[j].first = 0;
          vec[j].second = 0;

          merging = true;
        }
      }
    }

  } while (merging);

  vec.erase(std::remove_if(vec.begin(), vec.end(),
                           [](const std::pair<long long, long long> &p) {
                             return p.first == 0 && p.second == 0;
                           }),
            vec.end());
}

int main() {
  std::string filename{"input.txt"};
  std::vector<std::pair<long long, long long>> fresh{getFresh(filename)};

  /*
    for (const std::pair<long long, long long> &p : fresh) {
      std::cout << "(" << p.first << ", " << p.second << ")\n";
    }

    std::vector<long long> avaliable = getAvaliable(filename);

    for (long long n : avaliable) {
      std::cout << n << '\n';
    }
  */

  std::cout << "\nPart 1+:\n"
            << countAvaliableFresh(filename)
            << " available ingredient IDs are fresh.\n";

  mergeOverlap(fresh);

  std::cout << "\nPart 2:\n";
  long long count{};
  for (const std::pair<long long, long long> &p : fresh) {
    count += 1 + p.second - p.first;
  }
  std::cout << count << " available ingredient IDs are fresh.\n";

  return 0;
}
