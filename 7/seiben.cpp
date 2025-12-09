#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

enum class CellType {
  Start,
  Empty,
  Splitter,
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
    }
    return '?'; // fallback, should never happen
  }
};

struct Position {
  int row{};
  int col{};
};

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
    if (!file) {
      throw std::runtime_error("Failed to open file: " + filename);
    }

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

// ----------------- Beam simulation -----------------

enum class Direction { Up, Down, Left, Right };

struct Beam {
  Position pos{};
  Direction dir{Direction::Down};

  Beam() = default;
  Beam(Position p, Direction d) : pos{p}, dir{d} {}
};

Position moveOne(Position p, Direction d) {
  switch (d) {
  case Direction::Up:
    --p.row;
    break;
  case Direction::Down:
    ++p.row;
    break;
  case Direction::Left:
    --p.col;
    break;
  case Direction::Right:
    ++p.col;
    break;
  }
  return p;
}

class Game {
private:
  Map m_map;
  std::vector<Beam> m_beams;
  std::vector<std::vector<bool>> m_visited;
  int m_step{0};

public:
  explicit Game(const std::string &filename) : m_map(filename), m_step(0) {

    int rows = m_map.getRows();
    int cols = m_map.getCols();
    m_visited.assign(rows, std::vector<bool>(cols, false));

    Position start = m_map.getStart();

    // Beam starts BELOW the start point, going DOWN
    Position belowStart{start.row + 1, start.col};
    if (m_map.inBounds(belowStart.row, belowStart.col)) {
      m_beams.emplace_back(belowStart, Direction::Down);
      m_visited[belowStart.row][belowStart.col] = true;
    }
  }

  bool isDone() const { return m_beams.empty(); }

  void step() {
    ++m_step;
    std::vector<Beam> nextBeams;

    for (const Beam &beam : m_beams) {
      // Move the beam one cell in its direction
      Position nextPos = moveOne(beam.pos, beam.dir);

      // If it leaves the map, it disappears
      if (!m_map.inBounds(nextPos.row, nextPos.col)) {
        continue;
      }

      const Cell &cell = m_map.getCell(nextPos.row, nextPos.col);
      m_visited[nextPos.row][nextPos.col] = true;

      switch (cell.getType()) {
      case CellType::Empty:
      case CellType::Start:
        // Keep going in the same direction
        nextBeams.emplace_back(nextPos, beam.dir);
        break;

      case CellType::Splitter: {
        // Split into two beams: one to the left and one to the right
        Position leftPos = nextPos;
        Position rightPos = nextPos;

        leftPos.col -= 1;
        rightPos.col += 1;

        if (m_map.inBounds(leftPos.row, leftPos.col)) {
          nextBeams.emplace_back(leftPos, Direction::Left);
          m_visited[leftPos.row][leftPos.col] = true;
        }
        if (m_map.inBounds(rightPos.row, rightPos.col)) {
          nextBeams.emplace_back(rightPos, Direction::Right);
          m_visited[rightPos.row][rightPos.col] = true;
        }
        break;
      }
      }
    }

    m_beams = std::move(nextBeams);
  }

  void run() {
    while (!isDone()) {
      step();
    }
    std::cout << "Simulation finished after " << m_step << " steps.\n";
  }

  void printMap() const { m_map.print(); }

  // Optional: print all visited cells
  void printVisited() const {
    int rows = m_map.getRows();
    int cols = m_map.getCols();
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        std::cout << (m_visited[r][c] ? '#' : '.');
      }
      std::cout << '\n';
    }
  }
};

int main() {
  try {
    Game game("test.txt");

    std::cout << "Map:\n";
    game.printMap();
    game.run();

    std::cout << "Visited cells ( # = visited, . = not visited ):\n";
    game.printVisited();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
