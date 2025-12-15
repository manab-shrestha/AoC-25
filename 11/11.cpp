#include <cstddef>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Graph {
  std::vector<std::string> name{};
  std::vector<std::vector<int>> out{};
  std::unordered_map<std::string, int> id{};

  int getId(const std::string &s) {
    auto it{id.find(s)};
    if (it != id.end()) {
      return it->second;
    }
    int newId{static_cast<int>(name.size())};
    id.emplace(s, newId);
    name.push_back(s);
    out.push_back({});
    return newId;
  }

  void addEdge(const std::string &u, const std::string &v) {
    int a{getId(u)};
    int b{getId(v)};
    out[a].push_back(b);
  }
};

Graph readInput(const std::string &filename) {
  Graph g;

  std::ifstream file(filename);

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream iss(line);

    std::string node;
    iss >> node;

    if (!node.empty() && node.back() == ':') {
      node.pop_back();
    }

    std::string child;
    bool hasChild{false};

    while (iss >> child) {
      hasChild = true;
      if (child == "out") {
        break;
      }
      g.addEdge(node, child);
    }
    if (!hasChild || child == "out") {
      g.getId(node);
    }
  }
  return g;
}

long long countWays(int u, const Graph &g, std::vector<long long> &memo) {

  if (g.out[u].empty()) {
    return 1;
  }

  long long &cached{memo[u]};
  if (cached != -1) {
    return cached;
  }

  long long total{0};
  for (int v : g.out[u]) {
    total += countWays(v, g, memo);
  }

  cached = total;
  return total;
}

long long countWays2(int u, int a, int b, int mask, const Graph &g,
                     std::vector<std::vector<long long>> &memo) {

  if (u == a) {
    mask |= 1;
  }
  if (u == b) {
    mask |= 2;
  }

  long long &cached{memo[u][static_cast<std::size_t>(mask)]};
  if (cached != -1)
    return cached;

  if (g.out[u].empty()) {
    cached = (mask == 3) ? 1 : 0;
    return cached;
  }

  long long total{0};
  for (int v : g.out[u]) {
    total += countWays2(v, a, b, mask, g, memo);
  }

  cached = total;
  return total;
}

int main() {
  Graph g{readInput("input.txt")};

  int start{g.id.at("you")};
  std::vector<long long> memo(g.name.size(), -1);
  long long ways(countWays(start, g, memo));

  std::cout << ways << '\n';

  int start2{g.id.at("svr")};
  int dac{g.id.at("dac")};
  int fft{g.id.at("fft")};
  std::vector<std::vector<long long>> memo2(g.name.size());

  for (auto &row : memo2) {
    row = std::vector<long long>{-1, -1, -1, -1};
  };

  long long ways2(countWays2(start2, dac, fft, 0, g, memo2));
  std::cout << ways2 << '\n';

  return 0;
}
