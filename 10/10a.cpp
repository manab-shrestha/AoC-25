#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Mask {
  std::uint16_t bits{0};
  std::string repr{};
};

struct Problem {
  int B{0};
  std::uint32_t target{0U};
  std::vector<Mask> masks{};
};

static std::string stripSpaces(const std::string &s) {
  std::string out{};
  out.reserve(s.size());
  for (char ch : s) {
    if (!std::isspace(static_cast<unsigned char>(ch))) {
      out.push_back(ch);
    }
  }
  return out;
}

// Convention: RIGHTMOST char in [...] is bit 0.
// Example: ".##." -> 0110
static std::uint32_t parseTargetRightLSB(const std::string &targetStr) {
  const int B{static_cast<int>(targetStr.size())};
  std::uint32_t t{0U};

  for (int i{0}; i < B; ++i) {
    const char ch{targetStr[static_cast<std::size_t>(B - 1 - i)]};
    if (ch == '#') {
      t |= (1U << static_cast<std::uint32_t>(i));
    }
  }
  return t;
}

// Parse "(1,3,4)" into a bitmask; ignore indices >= B.
static Mask parseParenMask(const std::string &insideRaw, int B) {
  const std::string inside{stripSpaces(insideRaw)};
  std::uint32_t m{0U};

  if (!inside.empty()) {
    std::size_t i{0U};
    while (i < inside.size()) {
      bool neg{false};
      if (inside[i] == '-') {
        neg = true;
        ++i;
      }

      int val{0};
      bool anyDigit{false};

      while (i < inside.size() &&
             std::isdigit(static_cast<unsigned char>(inside[i]))) {
        anyDigit = true;
        val = val * 10 + (inside[i] - '0');
        ++i;
      }

      if (anyDigit && !neg) {
        if (val >= 0 && val < B) {
          m |= (1U << static_cast<std::uint32_t>(B - 1 - val));
        }
      }

      while (i < inside.size() && inside[i] != ',') {
        ++i;
      }
      if (i < inside.size() && inside[i] == ',') {
        ++i;
      }
    }
  }

  return Mask{static_cast<std::uint16_t>(m)};
}

static bool parseLineToProblem(const std::string &line, Problem &out) {
  if (line.empty()) {
    return false;
  }

  // treat '{' as end-of-line
  const std::size_t brace{line.find('{')};
  const std::string prefix{brace == std::string::npos ? line
                                                      : line.substr(0U, brace)};

  const std::size_t lb{prefix.find('[')};
  const std::size_t rb{prefix.find(']')};
  if (lb == std::string::npos || rb == std::string::npos || rb <= lb + 1U) {
    return false;
  }

  const std::string targetStr{prefix.substr(lb + 1U, rb - (lb + 1U))};
  const int B{static_cast<int>(targetStr.size())};
  if (B <= 0 || B > 24) {
    return false;
  }

  const std::uint32_t target{parseTargetRightLSB(targetStr)};

  std::vector<Mask> masks{};
  std::size_t pos{rb + 1U};

  while (true) {
    const std::size_t p1{prefix.find('(', pos)};
    if (p1 == std::string::npos) {
      break;
    }
    const std::size_t p2{prefix.find(')', p1 + 1U)};
    if (p2 == std::string::npos) {
      break;
    }

    const std::string inside{prefix.substr(p1 + 1U, p2 - (p1 + 1U))};
    masks.push_back(parseParenMask(inside, B));

    pos = p2 + 1U;
  }

  out = Problem{B, target, masks};
  return true;
}

static std::string toBits(std::uint32_t v, int B) {
  std::string s{};
  s.reserve(static_cast<std::size_t>(B));
  for (int i{B - 1}; i >= 0; --i) {
    s.push_back(((v >> static_cast<std::uint32_t>(i)) & 1U) ? '1' : '0');
  }
  return s;
}

static std::vector<Problem> readProblemsFromFile(const std::string &filename) {
  std::ifstream file{filename};
  if (!file) {
    throw std::runtime_error{"Failed to open file: " + filename};
  }

  std::vector<Problem> problems{};
  std::string line{};

  while (std::getline(file, line)) {
    Problem p{};
    if (parseLineToProblem(line, p)) {
      problems.push_back(p);
    }
  }

  return problems;
}

/*
 * Actual code not just input handling
 */

int fewestButtonPresses(const std::vector<Problem> &problems) {
  int count{};
  for (const auto &p : problems) {
    int B{p.B};
    const int maxState{1 << B};
    std::size_t M{p.masks.size()};

    std::vector<int> dist;
    dist.assign(maxState, -1);

    std::vector<int> queue{};
    queue.reserve(static_cast<std::size_t>(maxState));

    dist[0] = 0;
    queue.push_back(0);

    std::size_t head{0};

    int localCount{-1};

    while (head < queue.size()) {
      const int s{queue[head++]};

      if (static_cast<std::uint32_t>(s) == p.target) {
        localCount = dist[static_cast<std::size_t>(s)];
        break;
      }

      for (const Mask &m : p.masks) {
        const int next{s ^ static_cast<int>(m.bits)};
        if (dist[static_cast<std::size_t>(next)] == -1) {
          dist[static_cast<std::size_t>(next)] =
              dist[static_cast<std::size_t>(s)] + 1;
          queue.push_back(next);
        }
      }
    }

    count += localCount;

    std::cout << localCount << '\n';
  }
  return count;
}

int main() {
  const std::vector<Problem> problems{readProblemsFromFile("input.txt")};

  for (const auto &p : problems) {
    int B{p.B};
    std::cout << toBits(p.target, B) << '\n';
    for (const auto &masks : p.masks) {
      std::cout << '(' << toBits(masks.bits, B) << ")\n";
    }
  }

  int count{fewestButtonPresses(problems)};

  std::cout << count << '\n';

  return 0;
}
