#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>


std::pair<int, std::vector<int>> ParseBuses(std::string filename) {
    std::ifstream infile(filename);
    int departure_time;
    infile >> departure_time;

    std::vector<int> buses;
    std::string bus_string;
    while (std::getline(infile, bus_string, ',')) {
        if(bus_string != "x") {
            buses.push_back(std::stoi(bus_string));
        }
    }

    return {departure_time, buses};
}

std::pair<int, int> FindBestBusIdAndTimeToDeparture(int departure_time, const std::vector<int>& buses) {
    int lowest_dist_to_next = std::numeric_limits<int>::max();
    int best_bus = -1;
    
    for (const int bus : buses) {
        int mod = departure_time % bus;
        if (mod == 0) {
            // We can't do better.
            return {bus, 0};
        } else {
            int time_until = bus - mod;
            if (time_until < lowest_dist_to_next) {
                lowest_dist_to_next = time_until;
                best_bus = bus;
            }
        }
    }

    return {best_bus, lowest_dist_to_next};
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    const auto [depart_time, buses] = ParseBuses(argv[1]);
    const auto [bus_id, time_to_depart] = FindBestBusIdAndTimeToDeparture(depart_time, buses);
    int result = bus_id * time_to_depart;
    std::cout << "Result: " << result << std::endl;
}
