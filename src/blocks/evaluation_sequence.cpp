#include "evaluation_sequence.h"
#include <algorithm>
#include <limits>
#include <map>
#include <set>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <stack>
#include <unordered_set>
#include <vector>

namespace blocks {

namespace {

graph_t constructGraph(const Blocks_t blocks,
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

void dfs(uint u, const std::map<uint, std::vector<uint>>& graph,
         std::unordered_set<uint>& visited, std::stack<uint>& stack,
         std::unordered_set<uint>& onStack,
         std::set<std::pair<uint, uint>>& feedbackEdges) {
    visited.insert(u);
    stack.push(u);
    onStack.insert(u);
    if (graph.find(u) != graph.end()) {
        for (uint v : graph.at(u)) {
            if (visited.find(v) == visited.end()) {
                // Forward edge
                dfs(v, graph, visited, stack, onStack, feedbackEdges);
            } else if (onStack.find(v) != onStack.end()) {
                // Back edge (forms a cycle)
                feedbackEdges.insert({u, v});
            }
        }
    }
    stack.pop();
    onStack.erase(u);
}

std::set<std::pair<uint, uint>>
findFeedbackEdgeSet(const std::map<uint, std::vector<uint>>& graph) {
    std::unordered_set<uint> visited;
    std::unordered_set<uint> onStack;
    std::stack<uint> stack;
    std::set<std::pair<uint, uint>> feedbackEdges;

    for (const auto& entry : graph) {
        uint u = entry.first;
        if (visited.find(u) == visited.end()) {
            dfs(u, graph, visited, stack, onStack, feedbackEdges);
        }
    }

    return feedbackEdges;
}

void removeCycles(graph_t& graph) {
    auto feedbackEdges = findFeedbackEdgeSet(graph);
    for (const auto& [node, edge] : feedbackEdges) {
        graph[node].erase(
            std::find(graph[node].cbegin(), graph[node].cend(), edge));
    }
}

void collapseNodeEdges(const graph_t& graph, uint node,
                       std::vector<uint>& inConnectionsCount,
                       std::vector<uint>& result) {
    result.push_back(node);
    inConnectionsCount[node] =
        std::numeric_limits<uint>::max(); // prevents double checking a node
    for (uint outConnection : graph.at(node)) {
        --(inConnectionsCount[outConnection]);
        if (inConnectionsCount[outConnection] == 0) {
            collapseNodeEdges(graph, outConnection, inConnectionsCount, result);
        }
    }
}

std::vector<uint> topologicalSort(const graph_t& graph) {
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
            collapseNodeEdges(graph, node, inConnectionsCount, result);
        }
    }
    return result;
}

} // namespace

std::vector<uint> computeEvaluationSequence(graph_t graph) {
    removeCycles(graph);
    std::vector<uint> orderedNodes = topologicalSort(graph);
    return orderedNodes;
}

std::vector<uint> computeEvaluationSequence(const Blocks_t& blocks,
                                            const Connections_t& connections) {
    graph_t graph = constructGraph(blocks, connections);
    return computeEvaluationSequence(graph);
}

} // namespace blocks
