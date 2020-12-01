#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
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

// TODO: Update compiler and use std::optional.
std::pair<std::pair<int, int>, bool> IndicesWithSumInSubrange(std::vector<int> numbers, int target, int l, int r) {
    for (int sum = 0; sum != target; sum = numbers[l] + numbers[r]) {
        if (sum > target) {
            r--;
        } else {
            l++;
        }
        // Check the indexes haven't crossed.
        if (l >= r) {
            return {{0, 0}, false};
        }
    }

    return {{l, r}, true};
}

int FindAnswer(std::vector<int> numbers, int target) {
    std::sort(numbers.begin(), numbers.end());

    for (int i = 0; i < numbers.size() - 2; i++) {
        int value_here = numbers.at(i);
        auto maybe_pair = IndicesWithSumInSubrange(numbers, kTarget - value_here, i + 1, numbers.size() - 1);
        if (maybe_pair.second) {
            int l = maybe_pair.first.first;
            int r = maybe_pair.first.second;
            return value_here * numbers.at(l) * numbers.at(r);
        }
    }

    throw std::runtime_error("No matching triple of numbers found!");
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
