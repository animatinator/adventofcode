#include <bitset>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <variant>
#include <vector>


struct SetMask {
    std::string mask;
};

struct WriteValue {
    long long address;
    long long data;
};

typedef std::variant<SetMask, WriteValue> Instruction;

const std::string kWriteValueRegex = R"(mem\[([0-9]+)\] = ([0-9]+))";


std::runtime_error CreateInvalidMaskCharException(std::string mask, char bad_char) {
    std::stringstream msg;
    msg << "Mask '" << mask << "' contains invalid character '" << bad_char << "'!";
    return std::runtime_error(msg.str());
}

WriteValue ParseWriteValue(std::string line) {
    const std::regex instruction_regex(kWriteValueRegex);
    std::smatch match;

    if (std::regex_match(line, match, instruction_regex)) {
        return {std::stoll(match[1]), std::stoll(match[2])};
    } else {
        std::stringstream msg;
        msg << "Could not parse instruction: " << line;
        throw std::runtime_error(msg.str());
    }
}

std::vector<Instruction> ParseInstructions(std::string filename) {
    std::vector<Instruction> result;
    std::fstream infile(filename);

    std::string line;
    while (std::getline(infile, line)) {
        if (line.rfind("mask = ", 0) == 0) {
            result.push_back(SetMask{line.substr(7)});
        } else {
            result.push_back(ParseWriteValue(line));
        }
    }

    return result;
}

std::vector<long long> ComputeWriteList(long long address, std::string mask) {
    std::vector<long long> addresses = {address};

    for (int i = 0; i < mask.size(); ++i) {
        if (mask[i] == 'X') {
            long long one_here = 1LL << (mask.size() - i - 1);
            std::vector<long long> duplicated(addresses.begin(), addresses.end());
            for (auto& addr : addresses) {
                addr &= ~one_here;
            }
            for (auto& addr : duplicated) {
                addr |= one_here;
            }
            addresses.insert(addresses.begin(), duplicated.begin(), duplicated.end());
        } else if (mask[i] == '1') {
            for (auto& addr : addresses) {
                addr |= 1LL << (mask.size() - i - 1);
            }
        } else if (mask[i] != '0') {
            throw CreateInvalidMaskCharException(mask, mask[i]);
        }
    }
    return addresses;
}

long long ExecuteAndComputeMemorySum(const std::vector<Instruction>& instructions) {
    std::unordered_map<long long, long long> memory;
    std::string mask;

    for (const auto& instruction : instructions) {
        if (std::holds_alternative<SetMask>(instruction)) {
            SetMask set_mask = std::get<SetMask>(instruction);
            mask = set_mask.mask;
        } else if (std::holds_alternative<WriteValue>(instruction)) {
            WriteValue write_value = std::get<WriteValue>(instruction);
            std::vector<long long> write_list = ComputeWriteList(write_value.address, mask);
            for (const auto& address : write_list) {
                memory[address] = write_value.data;
            }
        }
    }

    long long result = 0;
    for (const auto& [addr, val] : memory) {
        result += val;
    }

    return result;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<Instruction> instructions = ParseInstructions(argv[1]);
    long long result = ExecuteAndComputeMemorySum(instructions);
    std::cout << "Sum of memory locations after execution: " << result << std::endl;
}