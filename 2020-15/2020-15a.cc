#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


std::vector<int> ParseNumbers(std::string filename) {
    std::vector<int> results;
    std::ifstream infile(filename);

    std::string num;
    while (std::getline(infile, num, ',')) {
        results.push_back(std::stoi(num));
    }

    return results;
}

int FindNthNumber(std::vector<int> starting_nums, int target) {
    std::unordered_map<int, int> last_seen;
    int next_num = starting_nums.at(0);

    for (int i = 0; i < target - 1; i++) {
        if (i < starting_nums.size() - 1) {
            last_seen[next_num] = i;
            next_num = starting_nums.at(i + 1);
        } else {
            if (last_seen.find(next_num) != last_seen.end()) {
                int when_seen = i - last_seen.at(next_num);
                last_seen[next_num] = i;
                next_num = when_seen;
            } else {
                last_seen[next_num] = i;
                next_num = 0;
            }
        }
    }    
    return next_num;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<int> starting_numbers = ParseNumbers(std::string(argv[1]));
    int num_at_2020 = FindNthNumber(starting_numbers, 2020);
    std::cout << "2020th number in the sequence: " << num_at_2020 << std::endl;
}