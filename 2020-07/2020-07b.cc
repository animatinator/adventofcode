#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>


const std::string kContainExpression = R"((.+?) bags contain (.+))";
const std::string kBagAndCount = R"((?:([0-9]+) (.+?) bags?(?:, )?))";


struct BagNode {
    BagNode(std::string bag_name) : name(bag_name) {}

    void AddChild(int count, std::shared_ptr<BagNode> parent) {
        contents.emplace_back(count, std::move(parent));
    }

    std::string name;
    std::vector<std::pair<int, std::shared_ptr<BagNode>>> contents;
};

class LuggageGraph {
  public:
    std::shared_ptr<BagNode> GetBag(std::string bag_name) {
        if (bags_.find(bag_name) == bags_.end()) {
            auto node = std::make_shared<BagNode>(bag_name);
            bags_.insert({bag_name, node});
            return node;
        } else {
            return bags_[bag_name];
        }
    }

    void BagContainsNBags(std::string container, int number, std::string child) {
        std::shared_ptr<BagNode> container_ptr = GetBag(container);
        std::shared_ptr<BagNode> child_ptr = GetBag(child);
        container_ptr->AddChild(number, std::move(child_ptr));
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<BagNode>> bags_;
};


std::unique_ptr<LuggageGraph> ParseLuggageGraph(const std::string& filename) {
    auto luggage_graph = std::make_unique<LuggageGraph>();
    std::ifstream infile(filename);
    const std::regex line_regex(kContainExpression);
    const std::regex bag_and_count_regex(kBagAndCount);
    std::smatch match;
    std::string line;

    while (std::getline(infile, line)) {
        if (!std::regex_match(line, match, line_regex)) {
            std::stringstream error_msg;
            error_msg << "Malformatted line: " << line;
            throw std::runtime_error(error_msg.str());
        }
        std::string parent_name = match[1];
        std::string bags_expression = match[2];
        std::smatch bag_and_count_match;

        while (std::regex_search(bags_expression, bag_and_count_match, bag_and_count_regex)) {
            int child_count = std::stoi(bag_and_count_match[1]);
            std::string child_name = bag_and_count_match[2];
            luggage_graph->BagContainsNBags(parent_name, child_count, child_name);
            bags_expression = bag_and_count_match.suffix().str();
        }
    }

    return luggage_graph;
}

int CountNumberOfBags(std::string bag_name, LuggageGraph* graph) {
    int total = 1;
    for (const auto [count, child] : graph->GetBag(bag_name)->contents) {
        int subtree_size = CountNumberOfBags(child->name, graph);
        total += count * subtree_size;
    }
    return total;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Must pass a file name to parse!";
        return 1;
    }

    auto graph = ParseLuggageGraph(std::string(argv[1]));
    int container_count = CountNumberOfBags("shiny gold", graph.get()) - 1;
    std::cout << "Number of child bags: " << container_count << std::endl;
}