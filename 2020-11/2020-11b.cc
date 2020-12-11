#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


constexpr bool kDebugLogging = false;

enum Direction {
    UP = 0,
    UPRIGHT = 1,
    RIGHT = 2,
    DOWNRIGHT = 3,
    DOWN = 4,
    DOWNLEFT = 5,
    LEFT = 6,
    UPLEFT = 7
};

const std::unordered_map<Direction, std::pair<int, int>> kDirectionsMap = {
    {Direction::UP, {0, -1}},
    {Direction::UPRIGHT, {1, -1}},
    {Direction::RIGHT, {1, 0}},
    {Direction::DOWNRIGHT, {1, 1}},
    {Direction::DOWN, {0, 1}},
    {Direction::DOWNLEFT, {-1, 1}},
    {Direction::LEFT, {-1, 0}},
    {Direction::UPLEFT, {-1, -1}}
};

enum TileType {
    FLOOR = 0,
    CHAIR = 1,
    FULL = 2,
};

struct Tile {
    Tile(TileType tile_type): type(tile_type) {}

    TileType type;
    std::unordered_map<Direction, int> nearest_chair;
};

std::pair<int, int> MultiplyDirectionalOffset(std::pair<int, int> direction, int multiple) {
    return {direction.first * multiple, direction.second * multiple};
}

std::string CreateInputTileErrorMessage(char tile) {
    std::stringstream stream;
    stream << "Invalid tile in input file: '" << tile << "'";
    return stream.str();
}

bool IsTile(const std::vector<std::vector<Tile>>& input, int x, int y, TileType type) {
    if (x >= 0 && x < input.at(0).size() && y >= 0 && y < input.size()) {
        return input.at(y).at(x).type == type;
    }
    return false;
}

bool IsChair(std::vector<std::vector<Tile>>& grid, int x, int y) {
    return IsTile(grid, x, y, TileType::CHAIR);
}

int GetDistanceToNearestChairInDirection(std::vector<std::vector<Tile>>& grid, int x, int y, Direction direction) {
    const auto [dx, dy] = kDirectionsMap.at(direction);
    int target_x = x + dx;
    int target_y = y + dy;
    if (target_x >= 0 && target_x < grid.at(0).size() && target_y >= 0 && target_y < grid.size()) {
        return IsChair(grid, target_x, target_y) ? 1 : grid.at(target_y).at(target_x).nearest_chair[direction] + 1;
    }
    return 1;
}

void SetNearestNeighbourChairs(std::vector<std::vector<Tile>>& grid) {
    // Start out with every grid square looking at its immediate neighbours.
    for (int x = 0; x < grid.at(0).size(); ++x) {
        for (int y = 0; y < grid.size(); ++y) {
            for (const auto& [direction, offset] : kDirectionsMap) {
                grid.at(y).at(x).nearest_chair[direction] = 1000;
            }
        }
    }

    // Top-left to bottom-right pass looking for left, topleft, top and topright nearest chairs.
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid.at(0).size(); ++x) {
            for (const auto& direction : {Direction::LEFT, Direction::UPLEFT, Direction::UP, Direction::UPRIGHT}) {
                grid.at(y).at(x).nearest_chair[direction] =
                    GetDistanceToNearestChairInDirection(grid, x, y, direction);
            }
        }
    }

    // Bottom-right to top-left pass looking for right, downright, down and downleft nearest chairs.
    for (int y = grid.size() - 1; y >= 0; --y) {
        for (int x = grid.at(0).size() - 1; x >= 0; --x) {
            for (const auto& direction : {Direction::RIGHT, Direction::DOWNRIGHT, Direction::DOWN, Direction::DOWNLEFT}) {
                grid.at(y).at(x).nearest_chair[direction] =
                    GetDistanceToNearestChairInDirection(grid, x, y, direction);
            }
        }
    }
}

std::vector<std::vector<Tile>> ParseGrid(std::string filename) {
    std::vector<std::vector<Tile>> result;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::vector<Tile> tile_line;

        for (const char c : line) {
            switch(c) {
                case '.':
                    tile_line.emplace_back(TileType::FLOOR);
                    break;
                case 'L':
                    tile_line.emplace_back(TileType::CHAIR);
                    break;
                case '#':
                    tile_line.emplace_back(TileType::FULL);
                    break;
                default:
                    throw std::runtime_error(CreateInputTileErrorMessage(c));
            }
        }

        result.push_back(tile_line);
    }

    SetNearestNeighbourChairs(result);

    return result;
}

bool IsFullChair(const std::vector<std::vector<Tile>>& input, int x, int y) {
    return IsTile(input, x, y, TileType::FULL);
}

TileType ComputeNewValue(const std::vector<std::vector<Tile>>& input, int x, int y) {

    Tile current_tile = input.at(y).at(x);
    TileType current_type = current_tile.type;
    int surrounding_full_chairs = 0;

    for (const auto& [direction, offset] : kDirectionsMap) {
        const auto [dx, dy] = MultiplyDirectionalOffset(offset, current_tile.nearest_chair[direction]);
        if (IsFullChair(input, x + dx, y + dy)) {
            ++surrounding_full_chairs;
        }
    }

    if (current_type == TileType::CHAIR && surrounding_full_chairs == 0) {
        return TileType::FULL;
    } else if (current_type == TileType::FULL && surrounding_full_chairs >= 5) {
        return TileType::CHAIR;
    }
    return current_type;
}

bool StepSimulationAndCheckIfChanged(
    const std::vector<std::vector<Tile>>& input, std::vector<std::vector<Tile>>& output) {
    bool changed = false;

    for (int x = 0; x < input.at(0).size(); x++) {
        for (int y = 0; y < input.size(); y++) {
            TileType value_here = input.at(y).at(x).type;
            TileType new_value = ComputeNewValue(input, x, y);
            if (value_here != new_value) {
                changed = true;
            }
            output.at(y).at(x).type = new_value;
        }
    }

    return changed;
}

int CountUnoccupiedSeats(const std::vector<std::vector<Tile>>& grid) {
    int result = 0;
    for (const auto& row : grid) {
        for (const auto& space : row) {
            if (space.type == TileType::FULL) {
                ++result;
            }
        }
    }
    return result;
}

void DumpGridForDebugging(const std::vector<std::vector<Tile>>& grid) {
    for (const auto& row : grid) {
        for (const auto& tile : row) {
            switch(tile.type) {
                case TileType::FLOOR:
                    std::cout << '.';
                    break;
                case TileType::CHAIR:
                    std::cout << 'L';
                    break;
                case TileType::FULL:
                    std::cout << '#';
                    break;
                default:
                    break;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int SimulateToCompletionAndCountUnoccupiedSeats(std::vector<std::vector<Tile>> grid) {
    std::vector<std::vector<Tile>> grid_alt(grid.begin(), grid.end());

    while (true) {
        if (kDebugLogging) DumpGridForDebugging(grid);
        StepSimulationAndCheckIfChanged(grid, grid_alt);
        if (kDebugLogging) DumpGridForDebugging(grid_alt);
        if (!StepSimulationAndCheckIfChanged(grid_alt, grid)) {
            break;
        }
    }

    return CountUnoccupiedSeats(grid);
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::vector<Tile>> grid = ParseGrid(std::string(argv[1]));
    int unoccupied_seats = SimulateToCompletionAndCountUnoccupiedSeats(grid);
    std::cout << "Unoccupied seats at fixed point: " << unoccupied_seats << std::endl;
}
