#include "block_system.h"
#include "evaluation_sequence.h"
#include "exceptions.h"
#include <algorithm>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace blocks {

BlockSystem::BlockSystem() : BlockComposite(0, 0) {}

void BlockSystem::evaluate() {
    if (shouldUpdateEvalSequence_) {
        updateEvaluationSequence();
        shouldUpdateEvalSequence_ = false;
    }
    // spdlog::info("Evaluating blocks");
    // spdlog::info("Sequence: {}", fmt::join(evalSequence_, ", "));
    // Set inputs
    for (uint i = 0; i < inputConnections_.size(); ++i) {
        auto port = inputConnections_[i];
        port.block->setInput(inputs_[i], port.port);
    }
    // Evaluate blocks
    for (auto blockIdx : evalSequence_) {
        auto block = blocks_[blockIdx];
        // spdlog::info("Evaluating: {}", block->getName());
        block->evaluate();
        for (const auto& connection : connections_[block]) {
            float value =
                connection.source.block->getOutput(connection.source.port);
            connection.target.block->setInput(value, connection.target.port);
        }
    }
    // Set outputs
    for (uint i = 0; i < outputConnections_.size(); ++i) {
        auto port = outputConnections_[i];
        outputs_[i] = port.block->getOutput(port.port);
    }
}

void BlockSystem::addBlock(std::shared_ptr<Block> block) {
    BlockComposite::addBlock(block);
    connections_.emplace(block, std::vector<Connection>());
}

void BlockSystem::removeBlock(std::shared_ptr<Block> block) {
    BlockComposite::removeBlock(block);
    auto it = connections_.find(block);
    connections_.erase(it);
    breakConnectionsTo(block);
    breakInputsOutputsTo(block);
}

void BlockSystem::addConnection(Connection connection) {
    if (!hasBlock(connection.source.block) ||
        !hasBlock(connection.target.block)) {
        throw invalid_operation_error(
            "Cannot connect: blocks from outside block system");
    }
    if (isPortConnected(connection.source, PortType::OUTPUT) ||
        isPortConnected(connection.target, PortType::INPUT)) {
        throw invalid_operation_error("Cannot connect: port already connected");
    }
    if (connection.source.port >= connection.source.block->getOutputSize() ||
        connection.target.port >= connection.target.block->getInputSize()) {
        throw invalid_operation_error("Cannot connect: port does not exist");
    }
    auto sourceBlock = connection.source.block;
    connections_[sourceBlock].emplace_back(connection);
    shouldUpdateEvalSequence_ = true;
}

void BlockSystem::removeConnection(Connection connection) {
    if (!hasConnection(connection)) {
        throw invalid_operation_error(
            "Cannot remove connection: the blocks are not connected");
    }
    auto sourceBlock = connection.source.block;
    auto& blockConnections = connections_[sourceBlock];
    auto it =
        std::find(blockConnections.begin(), blockConnections.end(), connection);
    blockConnections.erase(it);
    shouldUpdateEvalSequence_ = true;
}

void BlockSystem::addInput(Port port) {
    if (isPortConnected(port, PortType::INPUT) ||
        port.block->getInputSize() <= port.port) {
        throw invalid_operation_error("Cannot add input: invalid port");
    }
    inputs_.emplace_back(0.0f);
    inputConnections_.emplace_back(port);
}

void BlockSystem::removeInput(Port port) {
    auto it =
        std::find(inputConnections_.cbegin(), inputConnections_.cend(), port);
    if (it == inputConnections_.cend()) {
        throw invalid_operation_error(
            "Cannot remove input: port is not an input");
    }
    uint portIdx = it - inputConnections_.cbegin();
    inputConnections_.erase(inputConnections_.begin() + portIdx);
    inputs_.erase(inputs_.begin() + portIdx);
}

void BlockSystem::addOutput(Port port) {
    if (isPortConnected(port, PortType::OUTPUT) ||
        port.block->getOutputSize() <= port.port) {
        throw invalid_operation_error("Cannot add output: invalid port");
    }
    outputs_.emplace_back(0.0f);
    outputConnections_.emplace_back(port);
}

void BlockSystem::removeOutput(Port port) {
    auto it =
        std::find(outputConnections_.cbegin(), outputConnections_.cend(), port);
    if (it == outputConnections_.cend()) {
        throw invalid_operation_error(
            "Cannot remove output: port is not an output");
    }
    uint portIdx = it - outputConnections_.cbegin();
    outputConnections_.erase(outputConnections_.begin() + portIdx);
    outputs_.erase(outputs_.begin() + portIdx);
}

void BlockSystem::updateEvaluationSequence() {
    evalSequence_ = computeEvaluationSequence(blocks_, connections_);
}

bool BlockSystem::hasBlock(std::shared_ptr<Block> block) const {
    auto it = std::find(blocks_.cbegin(), blocks_.cend(), block);
    return (it != blocks_.cend());
}

bool BlockSystem::hasConnection(Connection connection) const {
    for (const auto& [block, connections] : connections_) {
        for (const auto& block_connection : connections) {
            if (block_connection == connection) {
                return true;
            }
        }
    }
    return false;
}

bool BlockSystem::isPortConnected(Port port, PortType type) const {
    // Check if port is used by a block connection
    for (const auto& [block, block_connections] : connections_) {
        for (const auto& connection : block_connections) {
            if ((connection.source == port && type == PortType::OUTPUT) ||
                (connection.target == port && type == PortType::INPUT)) {
                return true;
            }
        }
    }
    // Check if port is used by block system input/output
    return ((type == PortType::INPUT &&
             std::find(inputConnections_.cbegin(), inputConnections_.cend(),
                       port) != inputConnections_.cend()) ||
            (type == PortType::OUTPUT &&
             std::find(outputConnections_.cbegin(), outputConnections_.cend(),
                       port) != outputConnections_.cend()));
}

void BlockSystem::breakConnectionsTo(std::shared_ptr<Block> block) {
    auto findNextConnectionTo = [](std::shared_ptr<Block> block,
                                   const std::vector<Connection>& connections) {
        return std::find_if(connections.cbegin(), connections.cend(),
                            [block](Connection connection) {
                                return connection.target.block == block;
                            });
    };
    for (auto& [source_block, block_connections] : connections_) {
        auto it = findNextConnectionTo(block, block_connections);
        if (it != block_connections.cend()) {
            block_connections.erase(it);
            it = findNextConnectionTo(block, block_connections);
        }
    }
}
void BlockSystem::breakInputsOutputsTo(std::shared_ptr<Block> block) {
    for (const auto& port : inputConnections_) {
        if (port.block == block) {
            removeInput(port);
        }
    }
    for (const auto& port : outputConnections_) {
        if (port.block == block) {
            removeOutput(port);
        }
    }
}

} // namespace blocks
