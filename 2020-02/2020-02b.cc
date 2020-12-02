#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>


bool PasswordIsValid(std::string password, char required_character, int first_pos, int second_pos) {
    return (password[first_pos] == required_character) != (password[second_pos] == required_character);
}

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
        int first_pos = std::stoi(match[1]);
        int second_pos = std::stoi(match[2]);
        std::string char_string = match[3];
        std::string password = match[4];

        if (PasswordIsValid(password, char_string[0], first_pos - 1, second_pos - 1)) {
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