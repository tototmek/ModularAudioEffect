#include "evaluation_sequence.h"
#include <chrono>
#include <map>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace blocks {

namespace {

using graph_t = std::map<uint32_t, std::vector<uint32_t>>;

void printGraph(const graph_t& graph) {
    spdlog::info("Graph:");
    for (auto const& node : graph) {
        spdlog::info("{}:\t{}", node.first, fmt::join(node.second, ", "));
    }
}

graph_t
parseBlockStructure(const std::vector<std::reference_wrapper<Block>>& blocks) {
    graph_t graph;
    // Construct graph
    std::vector<uint32_t> connections;
    for (const auto& block : blocks) {
        connections.clear();
        for (const auto& outputPort : block.get().viewOutputPorts()) {
            std::optional<std::reference_wrapper<InputPort>> destination =
                outputPort.viewDestination();
            if (!destination.has_value()) {
                continue;
            }
            connections.emplace_back(
                destination.value().get().viewParent().getId());
        }
        graph.emplace(block.get().getId(), connections);
    }
    return graph;
}

void tryToCutRecursive(graph_t& graph, uint32_t nodeId,
                       std::vector<bool> visited) {
    visited[nodeId] = true;
    for (int i = graph[nodeId].size() - 1; i >= 0; --i) {
        uint32_t outConnection = graph[nodeId][i];
        if (visited[outConnection]) {
            // Remove connection from graph
            graph[nodeId].erase(graph[nodeId].begin() + i);

        } else {
            tryToCutRecursive(graph, outConnection, visited);
        }
    }
}

void disconnectCyclicConnections(graph_t& graph) {
    tryToCutRecursive(graph, 0, std::vector<bool>(graph.size(), false));
}

void collapseNodeConnections(const graph_t& graph, uint32_t node,
                             std::vector<uint32_t>& inConnectionsCount,
                             std::vector<uint32_t>& result) {
    result.push_back(node);
    inConnectionsCount[node] = 2137; // prevents double checking a node
    for (uint32_t outConnection : graph.at(node)) {
        --(inConnectionsCount[outConnection]);
        if (inConnectionsCount[outConnection] == 0) {
            collapseNodeConnections(graph, outConnection, inConnectionsCount,
                                    result);
        }
    }
}

std::vector<uint32_t> computeTopologicalOrdering(const graph_t& graph) {
    size_t nodesCount = graph.size();
    std::vector<uint32_t> result = {};
    std::vector<uint32_t> inConnectionsCount(nodesCount, 0);
    for (const auto& node : graph) {
        for (uint32_t outConnection : node.second) {
            ++(inConnectionsCount[outConnection]);
        }
    }
    for (size_t node = 0; node < nodesCount; ++node) {
        if (inConnectionsCount[node] == 0) {
            collapseNodeConnections(graph, node, inConnectionsCount, result);
        }
    }
    return result;
}

} // namespace

void EvaluationSequence::compute(
    const std::vector<std::reference_wrapper<Block>>& blocks) {
    spdlog::info("Computing operation evaluation order...");
    auto start = std::chrono::high_resolution_clock::now();

    spdlog::info("Parsing block structure...");
    // Step 0: Construct graph representation
    auto t1 = std::chrono::high_resolution_clock::now();
    graph_t graph = parseBlockStructure(blocks);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::info("Took {} microseconds", duration.count());

    spdlog::info("Parsed graph:");
    printGraph(graph);

    spdlog::info("Cutting cyclic connections...");
    // Step 1: Disconnect all cyclic dependenciens
    t1 = std::chrono::high_resolution_clock::now();
    disconnectCyclicConnections(graph);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::info("Took {} microseconds", duration.count());

    spdlog::info("Cut graph:");
    printGraph(graph);

    spdlog::info("Computing operation order...");
    // Step 2: Find topological ordering
    t1 = std::chrono::high_resolution_clock::now();
    auto orderedNodes = computeTopologicalOrdering(graph);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::info("Took {} microseconds", duration.count());

    blockSequence_.clear();
    blockSequence_.reserve(orderedNodes.size());
    for (size_t i = 0; i < orderedNodes.size(); ++i) {
        blockSequence_.push_back(blocks[orderedNodes[i]]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto deltatime =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    spdlog::info("\t -> Elapsed time: {} microseconds", deltatime.count());
    spdlog::info("\t -> Order: {}", fmt::join(orderedNodes, ", "));
}

void EvaluationSequence::evaluate() {
    for (auto& block : blockSequence_) {
        block.get().evaluate();
    }
}

} // namespace blocks
