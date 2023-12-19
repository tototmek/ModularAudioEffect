#include "evaluation_sequence.h"
#include <vector>

namespace blocks {

namespace {

struct graph_t {
    std::vector<uint32_t> nodeId;
    std::vector<std::vector<uint32_t>> inConnections;
    std::vector<std::vector<uint32_t>> outConnections;
};

graph_t
parseBlockStructure(const std::vector<std::reference_wrapper<Block>>& blocks) {
    uint32_t nextId = 0;
    graph_t result;
    for (const auto& block : blocks) {
        (void)block;
        result.nodeId.emplace_back(nextId++);
        std::vector<uint32_t> inConnections = {};
        result.inConnections.emplace_back(std::move(inConnections));
        std::vector<uint32_t> outConnections = {};
        result.outConnections.emplace_back(std::move(outConnections));
    }
    return result;
}

} // namespace

void EvaluationSequence::compute(
    const std::vector<std::reference_wrapper<Block>>& blocks) {
    // Step 0: Construct graph representation
    graph_t graph = parseBlockStructure(blocks);
    // Step 1: Disconnect all cyclic dependenciens
    // Step 2: Find topological ordering
}

void EvaluationSequence::evaluate() {
    for (auto& block : blockSequence_) {
        block.get().evaluate();
    }
}

} // namespace blocks
