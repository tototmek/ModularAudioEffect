#include "evaluation_sequence.h"
#include <chrono>
#include <limits>
#include <map>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <vector>

namespace blocks {

namespace {

using graph_t = std::map<uint, std::vector<uint>>;

void printGraph(const graph_t& graph) {
    spdlog::debug("Graph:");
    for (auto const& node : graph) {
        spdlog::debug("{}:\t{}", node.first, fmt::join(node.second, ", "));
    }
}

graph_t parseBlockStructure(const Blocks_t blocks,
                            const Connections_t& connections) {
    graph_t graph;
    std::map<std::shared_ptr<Block>, uint> block2IndexMap;
    for (uint i = 0; i < blocks.size(); ++i) {
        block2IndexMap.emplace(blocks[i], i);
    }
    std::vector<uint> blockConnections;
    for (const auto& [block, connection] : connections) {
        blockConnections.clear();
        for (const auto& element : connection) {
            uint blockIndex = block2IndexMap[element.target.block];
            blockConnections.emplace_back(blockIndex);
        }
        graph.emplace(block2IndexMap[block], blockConnections);
    }
    return graph;
}

void tryToCutRecursive(graph_t& graph, uint nodeId, std::vector<bool> visited) {
    visited[nodeId] = true;
    for (uint i = graph[nodeId].size(); i >= 1; --i) {
        uint outConnection = graph[nodeId][i - 1];
        if (visited[outConnection]) {
            // Remove connection from graph
            graph[nodeId].erase(graph[nodeId].begin() + i - 1);

        } else {
            tryToCutRecursive(graph, outConnection, visited);
        }
    }
}

void disconnectCyclicConnections(graph_t& graph) {
    tryToCutRecursive(graph, 0, std::vector<bool>(graph.size(), false));
}

void collapseNodeConnections(const graph_t& graph, uint node,
                             std::vector<uint>& inConnectionsCount,
                             std::vector<uint>& result) {
    result.push_back(node);
    inConnectionsCount[node] =
        std::numeric_limits<uint>::max(); // prevents double checking a node
    for (uint outConnection : graph.at(node)) {
        --(inConnectionsCount[outConnection]);
        if (inConnectionsCount[outConnection] == 0) {
            collapseNodeConnections(graph, outConnection, inConnectionsCount,
                                    result);
        }
    }
}

std::vector<uint> computeTopologicalOrdering(const graph_t& graph) {
    size_t nodesCount = graph.size();
    std::vector<uint> result = {};
    std::vector<uint> inConnectionsCount(nodesCount, 0);
    for (const auto& node : graph) {
        for (uint outConnection : node.second) {
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
    spdlog::debug("Computing operation evaluation order...");
    auto start = std::chrono::high_resolution_clock::now();

    spdlog::debug("Parsing block structure...");
    // Step 0: Construct graph representation
    auto t1 = std::chrono::high_resolution_clock::now();
    graph_t graph = parseBlockStructure(blocks, connections);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::debug("Took {} microseconds", duration.count());

    spdlog::debug("Parsed graph:");
    printGraph(graph);

    spdlog::debug("Cutting cyclic connections...");
    // Step 1: Disconnect all cyclic dependenciens
    t1 = std::chrono::high_resolution_clock::now();
    disconnectCyclicConnections(graph);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::debug("Took {} microseconds", duration.count());

    spdlog::debug("Cut graph:");
    printGraph(graph);

    spdlog::debug("Computing operation order...");
    // Step 2: Find topological ordering
    t1 = std::chrono::high_resolution_clock::now();
    auto orderedNodes = computeTopologicalOrdering(graph);
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
    spdlog::debug("Took {} microseconds", duration.count());
    auto end = std::chrono::high_resolution_clock::now();
    auto deltatime =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    spdlog::debug("\t -> Elapsed time: {} microseconds", deltatime.count());
    spdlog::debug("\t -> Order: {}", fmt::join(orderedNodes, ", "));

    return orderedNodes;
}

} // namespace blocks
