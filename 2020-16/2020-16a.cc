#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


const std::string kConstraintRegex = R"((.+): ([0-9]+)-([0-9]+) or ([0-9]+)-([0-9]+))";


// Represents a constraint of the form "l-ex_l or ex_r-r"
struct Constraint {
    int l;  // Left side
    int ex_l;  // Left side of the exclusion zone in the middle
    int ex_r;  // Right side of the exclusion zone in the middle
    int r;  // Right side
};

struct ProblemData {
    std::unordered_map<std::string, Constraint> constraints;
    std::vector<int> my_ticket;
    std::vector<std::vector<int>> near_tickets;
};

std::unordered_map<std::string, Constraint> ParseConstraints(const std::vector<std::string>& constraints) {
    std::unordered_map<std::string, Constraint> results;
    std::regex re(kConstraintRegex);
    std::smatch match;

    for (const auto& constraint_string : constraints) {
        if (std::regex_match(constraint_string, match, re)) {
            results[match[1]] = Constraint{std::stoi(match[2]), std::stoi(match[3]), std::stoi(match[4]), std::stoi(match[5])};
        }
    }
    return results;
}

std::vector<int> ParseTicket(const std::string& ticket) {
    std::vector<int> results;
    std::stringstream ticket_stream(ticket);
    std::string current_item;

    while (std::getline(ticket_stream, current_item, ',')) {
        results.push_back(std::stoi(current_item));
    }

    return results;
}

std::vector<std::vector<int>> ParseTickets(const std::vector<std::string>& tickets) {
    std::vector<std::vector<int>> results;
    for (const auto& ticket : tickets) {
        results.push_back(ParseTicket(ticket));
    }
    return results;
}

ProblemData ParseProblemData(const std::string& filename) {
    std::ifstream infile(filename);

    // Parse constraints
    std::vector<std::string> constraint_lines;
    std::string constraint_line;
    while (std::getline(infile, constraint_line) && !constraint_line.empty()) {
        constraint_lines.push_back(constraint_line);
    }
    auto constraints = ParseConstraints(constraint_lines);

    // Parse my ticket
    std::string ticket_header;
    std::string ticket_content;
    std::getline(infile, ticket_header);
    std::getline(infile, ticket_content);
    if (ticket_header != "your ticket:") {
        std::stringstream msg;
        msg << "Expected 'your ticket' header but found '" << ticket_header << "'!";
        throw std::runtime_error(msg.str());
    }
    auto my_ticket = ParseTicket(ticket_content);

    // Parse nearby tickets
    std::vector<std::vector<int>> near_tickets;
    // Ignore blank line.
    std::getline(infile, ticket_header);
    std::getline(infile, ticket_header);
    if (ticket_header != "nearby tickets:") {
        std::stringstream msg;
        msg << "Expected 'nearby tickets' header but found '" << ticket_header << "'!";
        throw std::runtime_error(msg.str());
    }
    while (std::getline(infile, ticket_content)) {
        near_tickets.push_back(ParseTicket(ticket_content));
    }

    return {constraints, my_ticket, near_tickets};
}

bool ValueMeetsConstraint(const int value, const Constraint& constraint) {
    return value >= constraint.l && value <= constraint.r && !(value > constraint.ex_l && value < constraint.ex_r);
}

int CountScanErrors(const std::vector<int>& ticket, const std::vector<Constraint>& constraints) {
    int errors = 0;

    for (const int value : ticket) {
        auto found = std::find_if(
            constraints.begin(), constraints.end(),
            [&value](const Constraint& c) {
                return ValueMeetsConstraint(value, c);
            });
        if (found == constraints.end()) {
            errors += value;
        }
    }

    return errors;
}

int ComputeScanErrorRate(const ProblemData& data) {
    int result = 0;

    std::vector<Constraint> constraints;
    constraints.reserve(data.constraints.size());
    for (const auto& kv : data.constraints) {
        constraints.push_back(kv.second);
    }

    for (const auto& ticket: data.near_tickets) {
        result += CountScanErrors(ticket, constraints);
    }

    return result;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    ProblemData data = ParseProblemData(std::string(argv[1]));
    int error_rate = ComputeScanErrorRate(data);
    std::cout << "Ticket scanning error rate: " << error_rate << std::endl;
}
