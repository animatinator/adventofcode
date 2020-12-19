#include <exception>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>


// Match n: [some rule].
const std::string kRuleRegex = R"regex((\d+): (.+))regex";
// Match "x" with x being some a-z letter.
const std::string kCharacterRegex = R"regex("([a-z])")regex";
// Match a branching rule like 1 2 | 3 4
const std::string kBranchRuleRegex = R"regex((.+?) \| (.+))regex";


struct CharRule {
    char c;
    CharRule(char ch): c(ch) {}
};

struct ListRule {
    std::vector<int> sub_rules;
    ListRule(std::vector<int> rules): sub_rules(rules) {}
};

struct BranchRule {
    ListRule left;
    ListRule right;
    BranchRule(ListRule l, ListRule r) : left(l), right(r) {}
};

typedef std::variant<CharRule, ListRule, BranchRule> Rule;

struct ProblemData {
    std::unordered_map<int, Rule> rules;
    std::vector<std::string> examples;
};

std::string DebugString(Rule rule) {
    if (std::holds_alternative<CharRule>(rule)) {
        auto char_rule = std::get<CharRule>(rule);
        std::stringstream result;
        result << "\"" << char_rule.c << "\"";
        return result.str();
    } else if (std::holds_alternative<ListRule>(rule)) {
        auto list_rule = std::get<ListRule>(rule);
        std::stringstream result;
        result << "[";
        for (const auto& rule : list_rule.sub_rules) {
            result << rule << ", ";
        }
        result << "]";
        return result.str();
    } else {
        auto branch_rule = std::get<BranchRule>(rule);
        std::stringstream result;
        result << "{";
        result << DebugString(branch_rule.left);
        result << " OR ";
        result << DebugString(branch_rule.right);
        result << "}";
        return result.str();
    }
}


ListRule ParseListRule(std::string input) {
    std::vector<int> sub_rules;

    std::stringstream input_stream(input);
    int rule;
    while (input_stream >> rule) {
        sub_rules.push_back(rule);
    }

    return {sub_rules};
}

Rule ParseRule(std::string rule) {
    std::regex char_regex(kCharacterRegex);
    std::regex branch_regex(kBranchRuleRegex);
    std::smatch match;

    if (std::regex_match(rule, match, char_regex)) {
        std::string value = match[1];
        return CharRule{value[0]};
    } else if (std::regex_match(rule, match, branch_regex)) {
        auto left = ParseListRule(match[1]);
        auto right = ParseListRule(match[2]);
        return BranchRule{left, right};
    } else {
        return ParseListRule(rule);
    }
}

std::pair<int, Rule> ParseRuleLine(std::string rule_line) {
    std::regex rule_regex(kRuleRegex);
    std::smatch match;

    if (std::regex_match(rule_line, match, rule_regex)) {
        int id = std::stoi(match[1]);
        return {id, ParseRule(match[2])};
    } else {
        std::stringstream msg;
        msg << "Invalid rule line in input: " << rule_line;
        throw std::runtime_error(msg.str());
    }
}

ProblemData ParseProblemData(std::string file_name) {
    std::ifstream infile(file_name);
    std::string line;

    std::unordered_map<int, Rule> rules;
    while (std::getline(infile, line) && !line.empty()) {
        rules.insert(ParseRuleLine(line));
    }

    std::vector<std::string> examples;
    while (std::getline(infile, line)) {
        examples.push_back(line);
    }

    return {rules, examples};
}


class Parser {
  public:
    Parser(std::string input, const std::unordered_map<int, Rule>& rules)
        : input_(input), parse_index_(0), rules_(rules) {}
    
    bool Matches() {
        return MatchRule(rules_.at(0)) && parse_index_ == input_.size();
    }

  private:
    bool MatchRule(Rule rule) {
        if (std::holds_alternative<CharRule>(rule)) {
            auto char_rule = std::get<CharRule>(rule);
            return input_[parse_index_++] == char_rule.c;
        } else if (std::holds_alternative<ListRule>(rule)) {
            auto list_rule = std::get<ListRule>(rule);
            for (const auto& sub_rule : list_rule.sub_rules) {
                if (!MatchRule(rules_.at(sub_rule))) {
                    return false;
                }
            }
            return true;
        } else {  // Branch rule.
            auto branch_rule = std::get<BranchRule>(rule);
            int current_parse_index = parse_index_;
            if (MatchRule(branch_rule.left)) {
                return true;
            }
            parse_index_ = current_parse_index;
            return MatchRule(branch_rule.right);
        }
        return true;
    }

    std::string input_;
    int parse_index_;
    const std::unordered_map<int, Rule>& rules_;
};

bool ExampleMatchesRules(const std::string& example, const std::unordered_map<int, Rule>& rules) {
    return Parser(example, rules).Matches();
}

int CountMatches(const std::vector<std::string>& examples, const std::unordered_map<int, Rule>& rules) {
    int matches = 0;

    for (const auto& example : examples) {
        std::cout << example << "? " << ExampleMatchesRules(example, rules) << std::endl;
        if (ExampleMatchesRules(example, rules)) {
            matches++;
        }
    }

    return matches;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    auto problem_data = ParseProblemData(std::string(argv[1]));
    int num_matches = CountMatches(problem_data.examples, problem_data.rules);
    std::cout << "Number of matches: " << num_matches << std::endl;
}
