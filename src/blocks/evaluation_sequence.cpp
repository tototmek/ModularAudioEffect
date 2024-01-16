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

graph_t parseBlockStructure(const Blocks_t blocks,
                            const Connections_t& connections) {
    graph_t graph;
    std::map<std::shared_ptr<Block>, uint> block2IndexMap;
    for (uint i = 0; i < blocks.size(); ++i) {
        block2IndexMap.emplace(blocks[i], i);
    }
    std::vector<uint32_t> blockConnections;
    for (auto& [block, connection] : connections) {
        blockConnections.clear();
        for (auto element : connection) {
            uint blockIndex = block2IndexMap[element.target.block];
            blockConnections.emplace_back(blockIndex);
        }
        graph.emplace(block2IndexMap[block], blockConnections);
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

std::vector<uint> computeEvaluationSequence(const Blocks_t& blocks,
                                            const Connections_t& connections) {
    spdlog::info("Computing operation evaluation order...");
    auto start = std::chrono::high_resolution_clock::now();

    spdlog::info("Parsing block structure...");
    // Step 0: Construct graph representation
    auto t1 = std::chrono::high_resolution_clock::now();
    graph_t graph = parseBlockStructure(blocks, connections);
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

    // sequence.clear();
    // sequence.reserve(orderedNodes.size());
    // for (size_t i = 0; i < orderedNodes.size(); ++i) {
    //     sequence.emplace_back(orderedNodes[i]);
    // }
    // sequence.emplace_back(1);

    auto end = std::chrono::high_resolution_clock::now();
    auto deltatime =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    spdlog::info("\t -> Elapsed time: {} microseconds", deltatime.count());
    spdlog::info("\t -> Order: {}", fmt::join(orderedNodes, ", "));

    return orderedNodes;
}

} // namespace blocks
