#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

constexpr int kTarget = 2020;

std::vector<int> ParseNumberListFile(std::string file_path) {
    std::vector<int> result;
    std::ifstream infile(file_path);
    int value;

    while (infile >> value) {
        result.push_back(value);
    }

    return result;
}

int FindAnswer(std::vector<int> numbers, int target) {
    std::sort(numbers.begin(), numbers.end());

    int l = 0;
    int r = numbers.size() - 1;

    for (int sum = 0; sum != target; sum = numbers[l] + numbers[r]) {
        if (sum > target) {
            r--;
        } else {
            l++;
        }
        // Check the indexes haven't crossed.
        if (l >= r) {
            throw std::runtime_error("No matching pair of numbers found!");
        }
    }

    return numbers[l] * numbers[r];
}

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<int> numbers = ParseNumberListFile(std::string(argv[1]));
    std::cout << "Result: " << FindAnswer(numbers, kTarget);
}
