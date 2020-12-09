#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>


bool ContainsNumbersSummingTo(std::vector<long long> vec, long long value) {
    int l = 0;
    int r = vec.size() - 1;

    for (long long sum = vec[l] + vec[r]; sum != value; sum = vec[l] + vec[r]) {
        if (sum > value) {
            r--;
        } else {
            l++;
        }
        // Check the indices haven't crossed.
        if (l >= r) {
            return false;
        }
    }

    return true;
}

long long FindIncorrectNumber(std::string filename, int pre_length) {
    std::queue<long long> recent_numbers;
    std::vector<long long> sorted_recent_numbers;

    std::ifstream infile(filename);
    long long current;

    while (infile >> current) {
        if (recent_numbers.size() < pre_length) {
            // Still processing the preamble.
            recent_numbers.push(current);
            sorted_recent_numbers.insert(
                std::upper_bound(
                    sorted_recent_numbers.begin(), sorted_recent_numbers.end(), current),
                current);
            continue;
        }

        if (!ContainsNumbersSummingTo(sorted_recent_numbers, current)) {
            return current;
        }

        long long oldest = recent_numbers.front();
        recent_numbers.pop();
        sorted_recent_numbers.erase(
            std::remove(sorted_recent_numbers.begin(), sorted_recent_numbers.end(), oldest),
            sorted_recent_numbers.end());
        recent_numbers.push(current);
        sorted_recent_numbers.insert(
            std::upper_bound(
                sorted_recent_numbers.begin(), sorted_recent_numbers.end(), current),
            current);
    }

    throw std::runtime_error("No incorrect number found in the input sequence!");
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Must pass a file name to parse and a preamble length!";
        return 1;
    }

    long long incorrect_number = FindIncorrectNumber(std::string(argv[1]), std::stoi(argv[2]));
    std::cout << "Incorrect number in sequence: " << incorrect_number << std::endl;
}