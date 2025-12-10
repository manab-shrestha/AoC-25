#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

enum class CellType {
  Start,
  Empty,
  Splitter,
  Beam,
};

class Cell {
private:
  CellType m_type{CellType::Empty};

public:
  Cell() = default;
  explicit Cell(CellType type) : m_type{type} {}

  CellType getType() const { return m_type; }
  void setType(CellType type) { m_type = type; }

  char toChar() const {
    switch (m_type) {
    case CellType::Start:
      return 'S';
    case CellType::Empty:
      return '.';
    case CellType::Splitter:
      return '^';
    case CellType::Beam:
      return '|';
    default:
      return '?';
    }
    return '?';
  }
};

struct Position {
  int row{};
  int col{};
};

Position operator+(Position a, Position b) {
  return Position{a.row + b.row, a.col + b.col};
}

class Map {
private:
  std::vector<std::vector<Cell>> m_cells{};
  int m_rows{};
  int m_cols{};
  Position m_start{};

  void initFromLines(const std::vector<std::string> &lines) {
    m_rows = static_cast<int>(lines.size());
    m_cols = m_rows > 0 ? static_cast<int>(lines[0].size()) : 0;

    m_cells.resize(m_rows, std::vector<Cell>(m_cols));
    for (int r{0}; r < m_rows; ++r) {
      const std::string &line{lines[r]};
      for (int c{0}; c < m_cols; ++c) {
        char ch{line[c]};
        CellType type;

        switch (ch) {
        case 'S':
          type = CellType::Start;
          m_start = Position{r, c};
          break;
        case '^':
          type = CellType::Splitter;
          break;
        case '|':
          type = CellType::Beam;
          break;
        default:
          type = CellType::Empty;
          break;
        }
        m_cells[r][c] = Cell{type};
      }
    }
  }

public:
  Map() = default;
  explicit Map(const std::string &filename) {
    std::ifstream file(filename);

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty()) {
        continue;
      }
      lines.push_back(line);
    }
    initFromLines(lines);
  }

  int getRows() const { return m_rows; }
  int getCols() const { return m_cols; }

  bool inBounds(int r, int c) const {
    return (r >= 0 && r < m_rows && c >= 0 && c < m_cols);
  }

  void setCell(Position pos, CellType type) {
    m_cells[pos.row][pos.col].setType(type);
  }
  const Cell &getCell(int r, int c) const { return m_cells[r][c]; }

  Position getStart() const { return m_start; }

  void print() const {
    for (int r{0}; r < m_rows; ++r) {
      for (int c{0}; c < m_cols; ++c) {
        std::cout << m_cells[r][c].toChar();
      }
      std::cout << '\n';
    }
  }
};

void simulateBeams(Map &map, int &splitCount) {
  int rows{map.getRows()};
  int cols{map.getCols()};

  map.print();

  map.setCell(map.getStart() + Position{1, 0}, CellType::Beam);

  for (int i{0}; i < rows; ++i) {
    for (int j{0}; j < cols; ++j) {
      if (map.getCell(i, j).getType() != CellType::Beam) {
        continue;
      }

      Position below{i + 1, j};

      if (!map.inBounds(below.row, below.col)) {
        continue;
      }

      CellType belowType{map.getCell(below.row, below.col).getType()};

      if (belowType == CellType::Empty) {
        map.setCell(below, CellType::Beam);
      } else if (belowType == CellType::Splitter) {
        Position leftDown{i + 1, j - 1};
        if (map.inBounds(leftDown.row, leftDown.col)) {
          map.setCell(leftDown, CellType::Beam);
        }

        Position rightDown{i + 1, j + 1};
        if (map.inBounds(rightDown.row, rightDown.col)) {
          map.setCell(rightDown, CellType::Beam);
        }
        ++splitCount;
      }
    }
    map.print();
  }
}

long long edgeFinder(int r, int c,
                     const std::vector<std::vector<std::size_t>> &nodeLoc,
                     std::vector<std::vector<long long>> &memo) {

  int rows{static_cast<int>(nodeLoc.size())};

  if (r == rows - 1) {
    return 1;
  }

  long long &cached{memo[r][c]};
  if (cached != -1) {
    return cached;
  }

  long long count{0};
  std::size_t x{nodeLoc[r][c]};

  for (int dx : {-1, 1}) {
    long long target{static_cast<long long>(x) + dx};
    if (target < 0) {
      continue;
    }

    bool attached{false};

    for (int nr{r + 1}; nr < rows && !attached; ++nr) {
      const auto &rowVec{nodeLoc[nr]};

      for (int k = 0; k < static_cast<int>(rowVec.size()); ++k) {
        if (static_cast<long long>(rowVec[k]) == target) {
          count += edgeFinder(nr, k, nodeLoc, memo);
          attached = true;
        }
      }
    }

    if (!attached && r < rows - 1) {
      count += 1;
    }
  }

  cached = count;
  return count;
}

long long countTimelines(const std::vector<std::vector<std::size_t>> &nodeLoc) {

  int rows{static_cast<int>(nodeLoc.size())};
  std::vector<std::vector<long long>> memo(rows);

  for (int r{0}; r < rows; ++r) {
    memo[r].assign(nodeLoc[r].size(), -1);
  }

  long long total{0};
  for (int c{0}; c < static_cast<int>(nodeLoc[0].size()); ++c) {
    total += edgeFinder(0, c, nodeLoc, memo);
  }

  return total;
}

std::vector<std::vector<std::size_t>> extractSplitterLocation(const Map &map) {
  int rows{map.getRows()};
  int cols{map.getCols()};

  std::vector<std::vector<std::size_t>> nodeLoc{};

  for (int i{0}; i < rows; ++i) {
    if (i % 2 != 0) {
      continue;
    }

    std::vector<std::size_t> rowBeams{};

    for (int j{0}; j < cols; ++j) {
      if (map.getCell(i, j).getType() == CellType::Splitter) {
        rowBeams.push_back(static_cast<std::size_t>(j));
      }
    }

    if (!rowBeams.empty()) {
      nodeLoc.push_back(std::move(rowBeams));
    }
  }

  return nodeLoc;
}

std::vector<std::size_t>
buildGhostBottomRow(const std::vector<std::vector<std::size_t>> &nodeLoc,
                    int cols) {
  const std::vector<std::size_t> &lastRow{nodeLoc.back()};

  std::vector<std::size_t> lastLine{};
  lastLine.reserve(2 * lastRow.size());

  for (std::size_t element : lastRow) {
    if (element > 0) {
      lastLine.push_back(element - 1);
    }
    if (element + 1 < static_cast<std::size_t>(cols)) {
      lastLine.push_back(element + 1);
    }
  }

  std::set<std::size_t> s{lastLine.begin(), lastLine.end()};
  lastLine.assign(s.begin(), s.end());

  return lastLine;
}

void printSplitterColumns(
    const std::vector<std::vector<std::size_t>> &nodeLoc) {
  for (std::size_t i{0}; i < nodeLoc.size(); ++i) {
    const std::vector<std::size_t> &vec{nodeLoc[i]};
    for (std::size_t j{0}; j < vec.size(); ++j) {
      std::cout << vec[j] << ' ';
    }
    std::cout << '\n';
  }
}

int main() {
  Map map{"test.txt"};
  int splitCount{0};

  simulateBeams(map, splitCount);

  std::vector<std::vector<std::size_t>> nodeLoc{extractSplitterLocation(map)};

  if (!nodeLoc.empty()) {
    int cols{map.getCols()};
    std::vector<std::size_t> ghostRow{buildGhostBottomRow(nodeLoc, cols)};
    nodeLoc.push_back(std::move(ghostRow));
  }

  std::cout << "\nThe beam splits " << splitCount << " times\n";
  ;

  // printSplitterColumns(nodeLoc);

  std::cout << "\nIn the many worlds interpretation there exist "
            << countTimelines(nodeLoc) << " timelines\n";

  return 0;
}

/*
6 8
5 7 9
4 6 8 10
3 5 7 8 9 11
2 4 6 7 8 10 12
1 3 4 5 7 8 10 11 13
0 2 4 6 8 10 11 12 14
*/
