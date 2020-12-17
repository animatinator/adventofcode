#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>


struct Bounds {
    Bounds(int bottom, int top): low(bottom), high(top), low_used(0), high_used(0) {}

    int Size() { return (high + 1) - low; }

    // When a value has been set to true, use this to update the low_used and high_used bounds to
    // match.
    void NotifyValueUsed(int value) {
        low_used = std::min(low_used, value);
        high_used = std::max(high_used, value);
    }

    // Returns a set of bounds just big enough to cover all values currently enclosed with one
    // space of padding on each end.
    Bounds CreateMinimalEnclosingBounds() {
        return {low_used - 1, high_used + 1};
    }

    int low;
    int low_used;
    int high_used;
    int high;
};

std::vector<std::tuple<int, int, int>> GetNeighbours(int x, int y, int z) {
    std::vector<std::tuple<int, int, int>> result;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                if (!(dx == 0 && dy == 0 && dz == 0)) {
                    result.push_back({x + dx, y + dy, z + dz});
                }
            }
        }
    }

    return result;
}

bool InBounds(int value, Bounds bounds) {
    return value >= bounds.low && value <= bounds.high;
}

class ConwayCube {
  public:
    // TODO(DO NOT SUBMIT): Make sure this is ultimately called using the filled bounds of the previous cube plus one.
    ConwayCube(Bounds b_x, Bounds b_y, Bounds b_z) : x_bounds_(b_x), y_bounds_(b_y), z_bounds_(b_z) {
        InitialiseZeroCube();
    }

    Bounds GetXBounds() const { return x_bounds_; }
    Bounds GetYBounds() const { return y_bounds_; }
    Bounds GetZBounds() const { return z_bounds_; }

    bool IsFilled(int x, int y, int z) const {
        if (!(InBounds(x, x_bounds_) && InBounds(y, y_bounds_) && InBounds(z, z_bounds_))) {
            return false;
        }
        int real_x = x - x_bounds_.low;
        int real_y = y - y_bounds_.low;
        int real_z = z - z_bounds_.low;

        return cube_.at(real_z).at(real_y).at(real_x);
    }

    void SetValue(int x, int y, int z, bool value) {
        if (!(InBounds(x, x_bounds_) && InBounds(y, y_bounds_) && InBounds(z, z_bounds_))) {
            std::stringstream msg;
            msg << "Position out of bounds: (" << x << ", " << y << ", " << z << ")!";
            throw std::runtime_error(msg.str());
        }

        if (value) {
            x_bounds_.NotifyValueUsed(x);
            y_bounds_.NotifyValueUsed(y);
            z_bounds_.NotifyValueUsed(z);
        }

        int real_x = x - x_bounds_.low;
        int real_y = y - y_bounds_.low;
        int real_z = z - z_bounds_.low;

        cube_.at(real_z).at(real_y).at(real_x) = value;
    }

    void UpdateFromPreviousCube(const ConwayCube& previous) {
        for (int z = z_bounds_.low; z <= z_bounds_.high; ++z) {
            for (int y = y_bounds_.low; y <= y_bounds_.high; ++y) {
                for (int x = x_bounds_.low; x < x_bounds_.high; ++x) {
                    auto neighbours = GetNeighbours(x, y, z);
                    int active_neighbours = 0;
                    for (const auto& [n_x, n_y, n_z] : neighbours) {
                        if (previous.IsFilled(n_x, n_y, n_z)) {
                            ++active_neighbours;
                        }
                    }

                    // If this voxel was previously filled...
                    if (previous.IsFilled(x, y, z)) {
                        if (active_neighbours == 2 || active_neighbours == 3) {
                            // Stay filled with 2 or 3 neighbours.
                            SetValue(x, y, z, true);
                        } else {
                            // Unfill otherwise.
                            SetValue(x, y, z, false);
                        }
                    } else {
                        // Otherise, fill if there are exactly three neighbours.
                        if (active_neighbours == 3) {
                            SetValue(x, y, z, true);
                        }
                    }
                }
            }
        }
    }

  private:
    void InitialiseZeroCube() {
        cube_ = std::vector<std::vector<std::vector<bool>>>();
        for (int z = 0; z < z_bounds_.Size(); ++z) {
            std::vector<std::vector<bool>> plane;
            for (int y = 0; y < y_bounds_.Size(); ++y) {
                std::vector<bool> row;
                for (int x = 0; x < x_bounds_.Size(); ++x) {
                    row.push_back(false);
                }
                plane.push_back(row);
            }
            cube_.push_back(plane);
        }
    }

    std::vector<std::vector<std::vector<bool>>> cube_;
    Bounds x_bounds_;
    Bounds y_bounds_;
    Bounds z_bounds_;
};

std::runtime_error CreateUnrecognisedCharacterException(char c) {
    std::stringstream msg;
    msg << "Unrecognised character in input data: " << c;
    return std::runtime_error(msg.str());
}

ConwayCube ParseInitialCube(std::string filename) {
    std::vector<std::vector<char>> data_grid;
    std::ifstream infile(filename);

    std::string line;
    while (std::getline(infile, line)) {
        std::vector<char> line_data;
        for (const char c : line) {
            line_data.push_back(c);
        }
        data_grid.push_back(line_data);
    }

    ConwayCube cube({0, data_grid.at(0).size() - 1}, {0, data_grid.size() - 1}, {0, 0});

    for (int x = 0; x < data_grid.at(0).size(); ++x) {
        for (int y = 0; y < data_grid.size(); ++y) {
            char c = data_grid.at(y).at(x);
            switch(c) {
                case '.':
                    break;
                case '#':
                    cube.SetValue(x, y, 0, true);
                    break;
                default:
                    throw CreateUnrecognisedCharacterException(c);
            }
        }
    }

    return cube;
}

ConwayCube EvaluateOneCycle(const ConwayCube& current) {
    Bounds new_x_bounds = current.GetXBounds().CreateMinimalEnclosingBounds();
    Bounds new_y_bounds = current.GetYBounds().CreateMinimalEnclosingBounds();
    Bounds new_z_bounds = current.GetZBounds().CreateMinimalEnclosingBounds();
    ConwayCube new_cube(new_x_bounds, new_y_bounds, new_z_bounds);
    new_cube.UpdateFromPreviousCube(current);
    return new_cube;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    ConwayCube first_cube = ParseInitialCube(std::string(argv[1]));
    first_cube = EvaluateOneCycle(first_cube);
    std::cout << "Parsed a cube. Lowest Z: " << first_cube.GetZBounds().low << ", highest X: " << first_cube.GetXBounds().high << std::endl;
}
