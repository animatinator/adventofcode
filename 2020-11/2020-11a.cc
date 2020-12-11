#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


constexpr bool kDebugLogging = false;


enum Tile {
    FLOOR = 0,
    CHAIR = 1,
    FULL = 2,
};

std::string CreateInputTileErrorMessage(char tile) {
    std::stringstream stream;
    stream << "Invalid tile in input file: '" << tile << "'";
    return stream.str();
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
                    tile_line.push_back(Tile::FLOOR);
                    break;
                case 'L':
                    tile_line.push_back(Tile::CHAIR);
                    break;
                case '#':
                    tile_line.push_back(Tile::FULL);
                    break;
                default:
                    throw std::runtime_error(CreateInputTileErrorMessage(c));
            }
        }

        result.push_back(tile_line);
    }

    return result;
}

bool IsFullChair(const std::vector<std::vector<Tile>>& input, int x, int y) {
    if (x >= 0 && x < input.at(0).size() && y >= 0 && y < input.size()) {
        return input.at(y).at(x) == Tile::FULL;
    }
    return false;
}

Tile ComputeNewValue(const std::vector<std::vector<Tile>>& input, int x, int y) {
    Tile current_value = input.at(y).at(x);
    int surrounding_full_chairs = 0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            if (IsFullChair(input, x + dx, y + dy)) {
                ++surrounding_full_chairs;
            }
        }
    }

    if (current_value == Tile::CHAIR && surrounding_full_chairs == 0) {
        return Tile::FULL;
    } else if (current_value == Tile::FULL && surrounding_full_chairs >= 4) {
        return Tile::CHAIR;
    }
    return current_value;
}

bool StepSimulationAndCheckIfChanged(const std::vector<std::vector<Tile>>& input, std::vector<std::vector<Tile>>& output) {
    bool changed = false;

    for (int x = 0; x < input.at(0).size(); x++) {
        for (int y = 0; y < input.size(); y++) {
            Tile value_here = input.at(y).at(x);
            Tile new_value = ComputeNewValue(input, x, y);
            if (value_here != new_value) {
                changed = true;
            }
            output.at(y).at(x) = new_value;
        }
    }

    return changed;
}

int CountUnoccupiedSeats(const std::vector<std::vector<Tile>>& grid) {
    int result = 0;
    for (const auto& row : grid) {
        for (const auto& space : row) {
            if (space == Tile::FULL) {
                ++result;
            }
        }
    }
    return result;
}

void DumpGridForDebugging(const std::vector<std::vector<Tile>>& grid) {
    for (const auto& row : grid) {
        for (const auto& tile : row) {
            switch(tile) {
                case Tile::FLOOR:
                    std::cout << '.';
                    break;
                case Tile::CHAIR:
                    std::cout << 'L';
                    break;
                case Tile::FULL:
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