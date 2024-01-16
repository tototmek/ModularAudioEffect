#include "block_system.h"
#include "evaluation_sequence.h"
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
        for (auto connection : connections_[block]) {
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

// TODO: make functions below safe, whenever the object is already in collection
// ( or is not but should)

void BlockSystem::addConnection(Connection connection) {
    auto sourceBlock = connection.source.block;
    connections_[sourceBlock].emplace_back(connection);
    shouldUpdateEvalSequence_ = true;
}

void BlockSystem::removeConnection(Connection connection) {
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

} // namespace blocks
