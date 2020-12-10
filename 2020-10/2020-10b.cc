#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


std::vector<int> ParseAdaptorJoltages(std::string filename) {
    std::vector<int> joltages;
    std::ifstream infile(filename);

    int current_joltage;
    while (infile >> current_joltage) {
        joltages.push_back(current_joltage);
    }

    return joltages;
}

long long FindNumberOfAdaptations(std::vector<int>& adaptor_joltages) {
    std::sort(adaptor_joltages.begin(), adaptor_joltages.end());
    std::unordered_map<int, long long> ways_to_make_joltage;
    ways_to_make_joltage[0] = 1;

    for (int joltage : adaptor_joltages) {
        long long ways_to_make_this_joltage = 0;
        for(int diff = 1; diff <= 3; ++diff) {
            if (ways_to_make_joltage.find(joltage - diff) != ways_to_make_joltage.end()) {
                ways_to_make_this_joltage += ways_to_make_joltage[joltage - diff];
            }
        }
        ways_to_make_joltage[joltage] = ways_to_make_this_joltage;
    }

    return ways_to_make_joltage[adaptor_joltages.at(adaptor_joltages.size() - 1)];
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<int> adaptor_joltages = ParseAdaptorJoltages(argv[1]);
    long long number_of_adaptations = FindNumberOfAdaptations(adaptor_joltages);
    std::cout << "Number of possible joltage adaptations: " << number_of_adaptations << std::endl;
}