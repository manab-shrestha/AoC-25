#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class UnionFind {

private:
  std::vector<std::size_t> parent;
  std::vector<std::size_t> rank;
  int setCount;

public:
  explicit UnionFind(std::size_t n) : parent(n), rank(n, 0), setCount(n) {
    for (std::size_t i{0}; i < n; ++i) {
      parent[i] = i;
    }
  }

  std::size_t find(std::size_t x) {
    if (parent[x] != x) {
      parent[x] = find(parent[x]);
    }
    return parent[x];
  }

  bool unite(size_t a, size_t b) {
    std::size_t rootA{find(a)};
    std::size_t rootB{find(b)};

    if (rootA == rootB) {
      return false;
    }

    if (rank[rootA] < rank[rootB]) {
      parent[rootA] = rootB;
    } else if (rank[rootA] > rank[rootB]) {
      parent[rootB] = rootA;
    } else {
      parent[rootB] = rootA;
      rank[rootA] += 1;
    }

    --setCount;
    return true;
  }

  bool same(int a, int b) { return find(a) == find(b); }

  int countSets() { return setCount; }
};

struct junctionBox {
  int label{};
  int x{};
  int y{};
  int z{};
  int circuit_num{0};
};

long long distance(const junctionBox &a, const junctionBox &b) {
  return (pow((a.x - b.x), 2) + pow((a.y - b.y), 2) + pow((a.z - b.z), 2));
}

std::vector<junctionBox> readBoxes(const std::string &filename) {
  std::vector<junctionBox> boxes{};

  std::ifstream file(filename);
  std::string line{};
  int label{0};

  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    int x{};
    int y{};
    int z{};
    char comma{','};

    std::istringstream iss(line);

    if (!(iss >> x >> comma >> y >> comma >> z) || comma != ',') {
      throw std::runtime_error("Bad input format: " + line);
    }

    boxes.emplace_back(junctionBox{label++, x, y, z, 0});
  }
  return boxes;
}

struct Triples {
  std::size_t i;
  std::size_t j;
  long long distance{};
};

std::vector<Triples> sortDistance(const std::vector<junctionBox> &boxes) {

  std::size_t n{boxes.size()};

  std::vector<Triples> distances;
  distances.reserve(0.5 * n * (n - 1));

  for (std::size_t i{0}; i < n; ++i) {
    for (std::size_t j{0}; (j < n) && (i > j); ++j) {
      distances.emplace_back(Triples{
          i,
          j,
          distance(boxes[i], boxes[j]),
      });
    }
  }
  std::sort(distances.begin(), distances.end(),
            [](const Triples &a, const Triples &b) {
              return a.distance < b.distance;
            });

  return distances;
}

std::vector<Triples> buildCircuits(std::vector<junctionBox> &boxes,
                                   const std::vector<Triples> &sortedDistances,
                                   std::size_t numConnections) {
  const std::size_t n{boxes.size()};

  if (n == 0 || numConnections == 0) {
    for (std::size_t k{0}; k < n; ++k) {
      boxes[k].circuit_num = static_cast<int>(k + 1);
    }
    return {};
  }

  UnionFind uf{n};

  std::vector<Triples> chosenEdges;
  chosenEdges.reserve(numConnections);

  std::size_t pairsProcessed{0};
  std::size_t madeConnections{0};

  for (const Triples &edge : sortedDistances) {
    if (pairsProcessed == numConnections) {
      break;
    }

    std::size_t a{edge.i};
    std::size_t b{edge.j};

    if (a >= n || b >= n) {
      ++pairsProcessed;
      continue;
    }

    if (uf.unite(a, b)) {
      chosenEdges.push_back(edge);
      ++madeConnections;
    }

    ++pairsProcessed;
  }

  std::cout << "Requested pairs: " << numConnections
            << ", processed: " << pairsProcessed
            << ", successful merges: " << madeConnections << '\n';

  std::unordered_map<std::size_t, int> rootToCircuit;
  int nextCircuitId{1};

  for (std::size_t k{0}; k < n; ++k) {
    std::size_t root{uf.find(k)};

    int circuitId;
    auto it{rootToCircuit.find(root)};
    if (it == rootToCircuit.end()) {
      circuitId = nextCircuitId;
      rootToCircuit[root] = circuitId;
      ++nextCircuitId;
    } else {
      circuitId = it->second;
    }

    boxes[k].circuit_num = circuitId;
  }

  return chosenEdges;
}

std::vector<std::pair<std::size_t, int>>
countLargestCircuits(const std::vector<junctionBox> &boxes) {
  std::size_t n{boxes.size()};

  std::vector<std::pair<std::size_t, int>> circuitSizes;
  circuitSizes.reserve(n);

  for (std::size_t i{1}; i <= n; ++i) {
    int count{0};
    for (const auto &box : boxes) {
      if (box.circuit_num == i)
        ++count;
    }
    circuitSizes.push_back(std::pair(i, count));
  }

  std::sort(
      circuitSizes.begin(), circuitSizes.end(),
      [](const std::pair<size_t, int> &a, const std::pair<size_t, int> &b) {
        if (a.second != b.second)
          return a.second > b.second;
        return a.first < b.first;
      });

  return circuitSizes;
}

int main() {
  std::vector<junctionBox> boxes{readBoxes("input.txt")};

  /*  for (const auto &position : positions) {
      std::cout << "position " << position.label << " has the coordiantes:\n"
                << "x: " << position.x << " y: " << position.y
                << " z: " << position.y << '\n';
    }
  */

  std::vector<Triples> sortedDistances{sortDistance(boxes)};

  /*for (const auto &d : sortedDistances) {
    std::cout << std::fixed << std::setprecision(15) << "Junction boxes ("
              << d.i << ',' << d.j << ") have a distance: " << d.distance
              << '\n';
  }*/

  for (int n{1}; n < 10000; ++n) {
    std::vector<Triples> wires{buildCircuits(boxes, sortedDistances, n)};
    std::vector<std::pair<std::size_t, int>> circuitSizes{
        countLargestCircuits(boxes)};

    for (const auto &circuit : circuitSizes) {
      if (circuit.second != 0) {
        std::cout << "Circuit " << circuit.first << " contains "
                  << circuit.second << " junction boxes." << '\n';
      }
    }
  }

  std::cout << boxes[sortedDistances[5041].i].x *
                   boxes[sortedDistances[5041].j].x
            << '\n';

  /* std::cout
       << "Multiplying together the sizes of the three largest circuits gives:
     "
       << circuitSizes[0].second * circuitSizes[1].second *
              circuitSizes[2].second
       << '\n'
       << sortedDistances.size() << '\n';
       */

  return 0;
}
