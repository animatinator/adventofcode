#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>


std::vector<std::optional<int>> ParseBuses(std::string filename) {
    std::ifstream infile(filename);
    int departure_time;
    infile >> departure_time;

    std::vector<std::optional<int>> buses;
    std::string bus_string;
    while (std::getline(infile, bus_string, ',')) {
        if (bus_string == "x") {
            buses.push_back({});
        } else {
            buses.push_back(std::stoi(bus_string));
        }
    }

    return buses;
}

int FindLeastFrequentBus(const std::vector<std::optional<int>>& buses) {
    int least_frequent = -1;
    int longest_wait = -1;

    for (int i = 0; i < buses.size(); ++i) {
        const auto& bus = buses.at(i);
        if (bus.has_value() && *bus > longest_wait) {
            longest_wait = *bus;
            least_frequent = i;
        }
    }

    return least_frequent;
}

bool BusesFormMagicalPatternAtTimestamp(const std::vector<std::optional<int>>& buses, long long timestamp) {
    for (long long i = 0; i < buses.size(); ++i) {
        const auto& cur_bus = buses.at(i);
        if (cur_bus.has_value()) {
            long long cur_bus_value = *cur_bus;
            if (((timestamp + i) % cur_bus_value) != 0) {
                return false;
            }
        }
    }
    return true;
}

long long FindEarliestMagicTimestamp(const std::vector<std::optional<int>>& buses) {
    long long timestamp = 0;
    int least_frequent_bus = FindLeastFrequentBus(buses);
    int longest_gap = *buses.at(least_frequent_bus);

    for (long long timestamp = longest_gap - least_frequent_bus; ; timestamp += longest_gap) {
        long long percent = 1000000;
        if (timestamp % percent == 0) {
            std::cout << timestamp << std::endl;
        }
        if (BusesFormMagicalPatternAtTimestamp(buses, timestamp)) {
            return timestamp;
        }
    }

    return longest_gap;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    const std::vector<std::optional<int>> buses = ParseBuses(argv[1]);
    const long long earliest_magic_timestamp = FindEarliestMagicTimestamp(buses);
    std::cout << "Earliest magic timestamp: " << earliest_magic_timestamp << std::endl;
}
