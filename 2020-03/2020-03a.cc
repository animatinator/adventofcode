#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


std::vector<bool> ParseLine(std::string line) {
    std::vector<bool> result;

    for (const char c : line) {
        switch(c) {
            case '.':
                result.push_back(false);
                break;
            case '#':
                result.push_back(true);
                break;
            default:
                std::stringstream stream;
                stream << "Input file contained illegal character: '" << c << "'";
                throw std::runtime_error(stream.str());
        }
    }

    return result;
}

std::vector<std::vector<bool>> LoadGridFromFile(std::string filename) {
    std::vector<std::vector<bool>> result;
    std::ifstream infile(filename);
    std::string line;

    while(std::getline(infile, line)) {
        result.push_back(ParseLine(line));
    }

    return result;
}

int CountTreeHits(std::vector<std::vector<bool>> map, int velx, int vely) {
    int trees_hit = 0;
    int x = 0;
    int y = 0;
    int width = map.at(0).size();
    int height = map.size();

    while (y < height) {
        if (map.at(y).at(x)) {
            ++trees_hit;
        }

        x = (x + velx) % width;
        y += vely;
    }

    return trees_hit;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    const auto grid = LoadGridFromFile(argv[1]);
    std::cout << "Hit " << CountTreeHits(grid, 3, 1) << " trees.";
}