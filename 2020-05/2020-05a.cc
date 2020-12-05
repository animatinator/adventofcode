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

std::pair<int, int> ParseRowAndSeat(const std::string& booking) {
    int seat_number = 0;
    int row_number = 0;
    int seat_add = 1;
    int row_add = 1;

    int parseIndex = booking.length() - 1;

    for (; booking[parseIndex] == 'L' || booking[parseIndex] == 'R'; --parseIndex) {
        if (booking[parseIndex] == 'R'){
            seat_number += seat_add;
        }
        seat_add *= 2;
    }

    for (; parseIndex >= 0 && (booking[parseIndex] == 'F' || booking[parseIndex] == 'B'); --parseIndex) {
        if (booking[parseIndex] == 'B') {
            row_number += row_add;
        }
        row_add *= 2;
    }

    if (parseIndex >= 0) {
        std::stringstream error_msg;
        error_msg << "Malformatted booking: " << booking << ". This should have been caught earlier!";
        throw std::runtime_error(error_msg.str());
    }

    return {row_number, seat_number};
}

int GetSeatIdMoreDescriptively(const std::string& booking) {
    auto [row, column] = ParseRowAndSeat(booking);
    return row * 8 + column;
}

int GetSeatIdQuickly(const std::string& booking) {
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

int GetHighestSeatId(const std::vector<std::string>& bookings) {
    int max = -1;

    for (const auto& booking : bookings) {
        int seat_id = GetSeatIdQuickly(booking);
        if (seat_id > max) {
            max = seat_id;
        }
    }

    return max;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<std::string> bookings = ParseBookings(std::string(argv[1]));
    int highest_seat_id = GetHighestSeatId(bookings);
    std::cout << "Highest seat ID: " << highest_seat_id << std::endl;
}