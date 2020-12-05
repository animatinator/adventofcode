#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>


const std::string kBookingRegex = R"(^(F|B){7}(L|R){3}$)";


std::vector<std::string> ParseBookings(const std::string& file_path) {
    std::vector<std::string> bookings;
    const std::regex booking_regex(kBookingRegex);
    std::smatch match;

    std::ifstream infile(file_path);
    std::string line;
    while (std::getline(infile, line)) {
        if (!std::regex_match(line, match, booking_regex)) {
            std::stringstream error_msg;
            error_msg << "Malformatted booking line: " << line;
            throw std::runtime_error(error_msg.str());
        }
        bookings.push_back(line);
    }

    return bookings;
}

int GetSeatId(const std::string& booking) {
    int result = 0;
    int add = 1;

    for (int i = booking.length() - 1; i >= 0; --i) {
        if (booking[i] == 'B' || booking[i] == 'R') {
            result += add;
        }
        add *= 2;
    }

    return result;
}

std::vector<int> GetSortedSeatIds(const std::vector<std::string>& bookings) {
    std::vector<int> result;

    for (const auto& booking : bookings) {
        int seat_id = GetSeatId(booking);
        result.push_back(seat_id);
    }

    std::sort(result.begin(), result.end());
    return result;
}

int GetMissingSeatId(const std::vector<int>& seat_ids) {
    int l = 0;
    int r = seat_ids.size();

    for (int mid = (l + r) / 2; mid != l; mid = (l + r) / 2) {
        int expected = seat_ids.at(0) + mid;
        if (seat_ids.at(mid) != expected) {
            r = mid;
        } else {
            l = mid;
        }
    }

    return seat_ids.at(0) + l + 1;
}

int GetMissingSeatId(const std::vector<std::string>& bookings) {
    const auto seat_ids = GetSortedSeatIds(bookings);
    return GetMissingSeatId(seat_ids);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::string> bookings = ParseBookings(std::string(argv[1]));
    int missing_seat_id = GetMissingSeatId(bookings);
    std::cout << "Missing seat ID: " << missing_seat_id << std::endl;
}