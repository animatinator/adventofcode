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
    long long mask;
    long long mask_overwrite;
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

SetMask ParseSetMask(std::string mask_value) {
    long long mask = 0;
    long long mask_overwrite = 0;

    for (int i = 0; i < mask_value.size(); ++i) {
        if (mask_value[i] == 'X') continue;

        mask |= 1LL << (mask_value.size() - i - 1);

        switch(mask_value[i]) {
            case '1':
                mask_overwrite |= 1LL << (mask_value.size() - i - 1);
                break;
            case '0':
                // Value is already zero here.
                break;
            default:
                throw CreateInvalidMaskCharException(mask_value, mask_value[i]);
        }
    }

    return {mask, mask_overwrite};
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
            result.push_back(ParseSetMask(line.substr(7)));
        } else {
            result.push_back(ParseWriteValue(line));
        }
    }

    return result;
}

long long ExecuteAndComputeMemorySum(const std::vector<Instruction>& instructions) {
    std::unordered_map<long long, long long> memory;
    long long mask;
    long long mask_overwrite;

    for (const auto& instruction : instructions) {
        if (std::holds_alternative<SetMask>(instruction)) {
            SetMask set_mask = std::get<SetMask>(instruction);
            mask = set_mask.mask;
            mask_overwrite = set_mask.mask_overwrite;
        } else if (std::holds_alternative<WriteValue>(instruction)) {
            WriteValue write_value = std::get<WriteValue>(instruction);
            long long value_to_write = (write_value.data & ~mask) + (mask_overwrite & mask);
            memory[write_value.address] = value_to_write;
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