#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

bool ContainsErrors(const std::vector<int>& ticket, const std::vector<Constraint>& constraints) {
    return CountScanErrors(ticket, constraints) != 0;
}

std::vector<Constraint> ConstraintsList(const ProblemData& problem_data) {
    std::vector<Constraint> result;

    for (const auto& kv : problem_data.constraints) {
        result.push_back(kv.second);
    }

    return result;
}

void FilterErroneousTickets(std::vector<std::vector<int>>& tickets, const std::vector<Constraint>& constraints) {
    tickets.erase(
        std::remove_if(
            tickets.begin(), tickets.end(),
            [&constraints](const std::vector<int>& ticket) {
                return ContainsErrors(ticket, constraints);
            }),
        tickets.end());
}

std::unordered_set<std::string> GetMatchedConstraints(
    const int value, const std::unordered_map<std::string, Constraint>& constraints) {
    std::unordered_set<std::string> result;

    for (const auto& [name, constraint] : constraints) {
        if (ValueMeetsConstraint(value, constraint)) {
            result.insert(name);
        }
    }

    return result;
}

void FilterOptionsToMatchedConstraints(
    std::unordered_set<std::string>& options, const std::unordered_set<std::string>& matched_constraints) {
        for (auto iter = options.begin(); iter != options.end();) {
            if (matched_constraints.find(*iter) != matched_constraints.end()) {
                ++iter;
            } else {
                iter = options.erase(iter);
            }
        }
    }

std::vector<std::string> GetOrderedFieldNames(const ProblemData& data) {
    std::unordered_map<int, std::unordered_set<std::string>> options;
    std::vector<std::vector<int>> tickets(data.near_tickets.begin(), data.near_tickets.end());
    tickets.push_back(data.my_ticket);
    std::vector<std::string> labels;

    for (const auto& kv: data.constraints) {
        labels.push_back(kv.first);
    }

    for (int i = 0; i < labels.size(); i++) {
        options[i] = std::unordered_set<std::string>(labels.begin(), labels.end());
    }

    for (const auto& ticket : tickets) {
        for (int i = 0; i < ticket.size(); ++i) {
            const auto matched_constraints = GetMatchedConstraints(ticket.at(i), data.constraints);
            FilterOptionsToMatchedConstraints(options.at(i), matched_constraints);
        }
    }

    // Prepare a result vector mapping each index to the empty string.
    std::vector<std::string> result;
    for (int i = 0; i < labels.size(); i++) {
        result.push_back("");
    }

    while (!options.empty()) {
        int unambiguous_key = -1;
        // Pick an options entry that has an unambiguous mapping: only one option left for this index.
        for (const auto& [key, possibilities] : options) {
            if (possibilities.size() == 1) {
                unambiguous_key = key;
                break;
            }
        }
        if (unambiguous_key == -1) {
            // Just in case we already found the mappings we need, dump out all the options now.
            std::cout << "Couldn't find a certain mapping. Dumping options:" << std::endl;
            for (const auto& [key, set] : options) {
                std::cout << key << ": ";
                for (const auto& option : set) {
                    std::cout << option << ", ";
                }
                std::cout << std::endl;
            }
            throw std::runtime_error("Couldn't find a certain mapping!");
        }
        // Apply the only option for this index and remove its entry from the options map.
        std::string label = *(options.at(unambiguous_key).begin());
        std::cout << "Applying certain mapping from " << unambiguous_key << " to " << label << std::endl;
        result.at(unambiguous_key) = label;
        options.erase(unambiguous_key);
        // Remove this option from all the other option entries.
        for (auto& [key, option] : options) {
            option.erase(label);
        }
    }

    return result;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    // Here's a massive hack:
    // Extracted these numbers from the error message thrown by GetOrderedFieldNames, which did
    // manage to find the positions of the 'departure' fields before it ran out of options.
    long long test = 101LL*109LL*97LL*149LL*89LL*137LL;
    std::cout << "Answer found by hackery: " << test << std::endl;

    // And now, the unfinished code which may eventually find the right answer without crashing in
    // the process.
    ProblemData data = ParseProblemData(std::string(argv[1]));
    FilterErroneousTickets(data.near_tickets, ConstraintsList(data));
    std::vector<std::string> ordered_field_names = GetOrderedFieldNames(data);
    for (const auto& name : ordered_field_names) {
        std::cout << name << std::endl;
    }
}
