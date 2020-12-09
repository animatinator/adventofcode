#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>


std::vector<long long> ReadInput(std::string filename) {
    std::vector<long long> result;
    std::ifstream infile(filename);
    long long current;

    while (infile >> current) {
        result.push_back(current);
    }

    return result;
}

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

long long FindIncorrectNumber(const std::vector<long long>& input, int pre_length) {
    std::queue<long long> recent_numbers;
    std::vector<long long> sorted_recent_numbers;

    for (long long current : input) {
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

std::vector<long long> FindContiguousNumbersWithSum(const std::vector<long long>& input, long long target) {
    int l = 0;
    int r = 0;
    long long sum = input.at(0);

    while ((sum != target || r - l < 1) && r < input.size()) {
        if (sum < target) {
            ++r;
            sum += input.at(r);
        } else if (sum > target) {
            sum -= input.at(l);
            ++l;
        }
        if (l > r) {
            throw std::runtime_error("Indices crossed whilst finding contiguous numbers with sum!");
        }
    }

    return std::vector<long long>(input.begin() + l, input.begin() + r + 1);
}

long long ComputeEncryptionWeakness(const std::vector<long long>& input, long long incorrect_number) {
    std::vector<long long> contiguous_sum_range = FindContiguousNumbersWithSum(input, incorrect_number);
    return *std::min_element(contiguous_sum_range.begin(), contiguous_sum_range.end())
        + *std::max_element(contiguous_sum_range.begin(), contiguous_sum_range.end());
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Must pass a file name to parse and a preamble length!";
        return 1;
    }

    std::vector<long long> input = ReadInput(std::string(argv[1]));
    long long incorrect_number = FindIncorrectNumber(input, std::stoi(argv[2]));
    long long encryption_weakness = ComputeEncryptionWeakness(input, incorrect_number);
    std::cout << "Incorrect number in sequence: " << incorrect_number << std::endl;
    std::cout << "Encryption weakness: " << encryption_weakness << std::endl;
}
