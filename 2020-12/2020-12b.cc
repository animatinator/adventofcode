#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


enum Direction {
    EAST = 0,
    SOUTH = 1,
    WEST = 2,
    NORTH = 3,
    FORWARD = 4,
};

std::pair<int, int> RotateWaypoint(int x, int y, int angle) {
    int n_turns = (angle / 90) % 4;
    // Fix negative rotation.
    n_turns = (n_turns + 4) % 4;

    for (int i = 0; i < n_turns; ++i) {
        int tmp = x;
        x = y;
        y = -tmp;
    }

    return {x, y};
}

const std::unordered_map<Direction, std::pair<int, int>> kDirectionOffsets = {
    {Direction::EAST, {1, 0}},
    {Direction::SOUTH, {0, -1}},
    {Direction::WEST, {-1, 0}},
    {Direction::NORTH, {0, 1}}};

struct Instruction {
    enum Type {
        EAST = 0,
        SOUTH = 1,
        WEST = 2,
        NORTH = 3,
        FORWARD = 4,
        LEFT = 5,
        RIGHT = 6,
    };

    Instruction(Type t, int a): type(t), arg(a) {}

    Type type;
    int arg;
};

const std::unordered_map<char, Instruction::Type> kInstructionTypeMap = {
    {'N', Instruction::Type::NORTH},
    {'S', Instruction::Type::SOUTH},
    {'E', Instruction::Type::EAST},
    {'W', Instruction::Type::WEST},
    {'F', Instruction::Type::FORWARD},
    {'L', Instruction::Type::LEFT},
    {'R', Instruction::Type::RIGHT}};

class Ferry {
  public:
    void Rotate(int angle) {
        const auto [n_x, n_y] = RotateWaypoint(w_x_, w_y_, angle);
        w_x_ = n_x;
        w_y_ = n_y;
        std::cout << "Waypoint rotated to (" << w_x_ << ", " << w_y_ << ")." << std::endl;
    }

    void Move(Direction direction, int amount) {
        if (direction == Direction::FORWARD) {;
            // Move to the waypoint 'amount' times.
            x_ += w_x_ * amount;
            y_ += w_y_ * amount;
        } else {
            // Move the waypoint in the specified direction.
            const auto& [dx, dy] = kDirectionOffsets.at(direction);
            w_x_ += dx * amount;
            w_y_ += dy * amount;
        }

        std::cout << "Move happened. Ship is at (" << x_ << ", " << y_ << ");"
                  << " waypoint at (" << w_x_ << ", " << w_y_ << ")." << std::endl;
    }

    std::pair<int, int> GetPosition() { return {x_, y_}; }

  private:
    int x_ = 0;
    int y_ = 0;
    // Waypoint starts at 10 East, 1 North.
    int w_x_ = 10;
    int w_y_ = 1;
    Direction facing_ = Direction::EAST;
};

std::vector<Instruction> ParseInstructions(std::string filename) {
    std::vector<Instruction> instructions;
    std::ifstream infile(filename);

    char instr_code;
    int arg;
    while (infile >> instr_code >> arg) {
        if (kInstructionTypeMap.find(instr_code) == kInstructionTypeMap.end()) {
            std::stringstream estream;
            estream << "Invalid instruction code: '" << instr_code << "'!";
            throw std::runtime_error(estream.str());
        }
        instructions.emplace_back(kInstructionTypeMap.at(instr_code), arg);
    }

    return instructions;
}

std::pair<int, int> ExecuteInstructions(const std::vector<Instruction>& instructions) {
    Ferry ferry;

    for (const Instruction& instruction : instructions) {
        switch(instruction.type) {
            case Instruction::Type::EAST:
                ferry.Move(Direction::EAST, instruction.arg);
                break;
            case Instruction::Type::SOUTH:
                ferry.Move(Direction::SOUTH, instruction.arg);
                break;
            case Instruction::Type::WEST:
                ferry.Move(Direction::WEST, instruction.arg);
                break;
            case Instruction::Type::NORTH:
                ferry.Move(Direction::NORTH, instruction.arg);
                break;
            case Instruction::Type::FORWARD:
                ferry.Move(Direction::FORWARD, instruction.arg);
                break;
            case Instruction::Type::LEFT:
                ferry.Rotate(-instruction.arg);
                break;
            case Instruction::Type::RIGHT:
                ferry.Rotate(instruction.arg);
                break;
        }
    }

    return ferry.GetPosition();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    std::vector<Instruction> instructions = ParseInstructions(std::string(argv[1]));
    const auto [x, y] = ExecuteInstructions(instructions);
    std::cout << "Final position: (" << x << ", " << y << ")." << std::endl;
    std::cout << "Manhattan distance: " << std::abs(x) + std::abs(y) << "." << std::endl;
}
