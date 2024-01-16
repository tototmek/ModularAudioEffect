#include "block_system.h"
#include "evaluation_sequence.h"
#include "exceptions.h"
#include <algorithm>

namespace blocks {

BlockSystem::BlockSystem() : BlockComposite(0, 0) {}

void BlockSystem::evaluate() {
    if (shouldUpdateEvalSequence_) {
        updateEvaluationSequence();
        shouldUpdateEvalSequence_ = false;
    }
    // Set inputs
    for (uint i = 0; i < inputConnections_.size(); ++i) {
        auto port = inputConnections_[i];
        port.block->setInput(inputs_[i], port.port);
    }
    // Evaluate blocks
    for (auto blockIdx : evalSequence_) {
        auto block = blocks_[blockIdx];
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
    inputs_.emplace_back(0.0f);
    inputConnections_.emplace_back(port);
}

void BlockSystem::removeInput(Port port) {
    uint portIdx =
        std::find(inputConnections_.begin(), inputConnections_.end(), port) -
        inputConnections_.begin();
    inputConnections_.erase(inputConnections_.begin() + portIdx);
    inputs_.erase(inputs_.begin() + portIdx);
}

void BlockSystem::addOutput(Port port) {
    outputs_.emplace_back(0.0f);
    outputConnections_.emplace_back(port);
}

void BlockSystem::removeOutput(Port port) {
    uint portIdx =
        std::find(outputConnections_.begin(), outputConnections_.end(), port) -
        outputConnections_.begin();
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
    for (const auto& [block, block_connections] : connections_) {
        for (const auto& connection : block_connections) {
            if ((connection.source == port && type == PortType::OUTPUT) ||
                (connection.target == port && type == PortType::INPUT)) {
                return true;
            }
        }
    }
    return false;
}

} // namespace blocks
