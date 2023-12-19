#include "evaluation_sequence.h"
#include <map>
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
    std::map<uint32_t, size_t> blockNodeIndexRemap;

    // Find BlockID -> graph node index correspondence
    size_t index = 0;
    for (const auto& block : blocks) {
        blockNodeIndexRemap.emplace(block.get().getId(), index++);
    }

    // Construct graph
    for (const auto& block : blocks) {
        result.nodeId.emplace_back(nextId++);
        std::vector<uint32_t> inConnections = {};
        result.inConnections.emplace_back(
            std::move(inConnections)); // Empty at this step
        std::vector<uint32_t> outConnections = {};
        for (const auto& outputPort : block.get().viewOutputPorts()) {
            std::optional<std::reference_wrapper<InputPort>> destination =
                outputPort.viewDestination();
            if (!destination.has_value()) {
                continue;
            }
            uint32_t destinationBlockId =
                destination.value().get().viewParent().getId();
            size_t destinationNodeIndex =
                blockNodeIndexRemap.at(destinationBlockId);
            outConnections.emplace_back(destinationNodeIndex);
        }
        result.outConnections.emplace_back(std::move(outConnections));
    }

    // Parse graph to get input connections
    for (size_t i = 0; i < result.nodeId.size(); ++i) {
        uint32_t id = result.nodeId[i];
        for (uint32_t outputConnection : result.outConnections[i]) {
            result.inConnections[outputConnection].push_back(id);
        }
    }

    return result;
}

} // namespace

void EvaluationSequence::compute(
    const std::vector<std::reference_wrapper<Block>>& blocks) {
    // Step 0: Construct graph representation
    graph_t graph = parseBlockStructure(blocks);

    printf("Computed connection graph\nID\t\tIN\t\tOUT\n");
    for (size_t i = 0; i < graph.nodeId.size(); ++i) {
        printf("%d:\t\t", graph.nodeId[i]);
        for (uint32_t inConnection : graph.inConnections[i]) {
            printf("%d, ", inConnection);
        }
        printf("\t\t");
        for (uint32_t outConnection : graph.outConnections[i]) {
            printf("%d, ", outConnection);
        }
        printf("\n");
    }
    // Step 1: Disconnect all cyclic dependenciens
    // Step 2: Find topological ordering
}

void EvaluationSequence::evaluate() {
    for (auto& block : blockSequence_) {
        block.get().evaluate();
    }
}

} // namespace blocks
