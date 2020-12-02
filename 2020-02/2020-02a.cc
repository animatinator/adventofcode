#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>


int CountValidPasswords(std::string file_name) {
    std::regex line_regex(R"((\d+)-(\d+)\s([a-z]):\s([a-z]+))");
    std::ifstream infile(file_name);
    std::string line;
    std::smatch match;
    int valid_passwords = 0;

    while (std::getline(infile, line)) {
        if (!(std::regex_match(line, match, line_regex))) {
            std::stringstream error_msg;
            error_msg << "Line didn't match the expected pattern: " << line;
            throw std::runtime_error(error_msg.str());
        }
        int min = std::stoi(match[1]);
        int max = std::stoi(match[2]);
        std::string char_string = match[3];
        std::string password = match[4];

        int occurrences = std::count(password.begin(), password.end(), char_string[0]);

        if (min <= occurrences && occurrences <= max) {
            std::cout << password << std::endl;
            ++valid_passwords;
        }
    }

    return valid_passwords;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::cout << CountValidPasswords(std::string(argv[1]));
}