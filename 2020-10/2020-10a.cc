#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
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

int ComputeDifferenceProduct(std::vector<int>& adaptor_joltages) {
    std::sort(adaptor_joltages.begin(), adaptor_joltages.end());
    int last_joltage = 0;
    int unit_diffs = 0;
    // There's always a three-jolt difference between the last adaptor and the device.
    int three_diffs = 1;

    for (const int joltage : adaptor_joltages) {
        switch (joltage - last_joltage) {
            case 1:
                ++unit_diffs;
                break;
            case 3:
                ++three_diffs;
                break;
            default:
                continue;
        }
        last_joltage = joltage;
    }

    return unit_diffs * three_diffs;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<int> adaptor_joltages = ParseAdaptorJoltages(argv[1]);
    int difference_product = ComputeDifferenceProduct(adaptor_joltages);
    std::cout << "Product of 1-jolt and 3-jolt differences: " << difference_product << std::endl;
}