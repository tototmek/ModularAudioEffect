#include "block.h"

namespace blocks {

uint32_t Block::nextId_ = 0;

Block::Block(const std::vector<InputPort>& inputPorts,
             const std::vector<OutputPort>& outputPorts)
    : id_(nextId_++), inPorts_(inputPorts), outPorts_(outputPorts) {}

Block::Block(std::vector<InputPort>&& inputPorts,
             std::vector<OutputPort>&& outputPorts)
    : id_(nextId_++), inPorts_(std::move(inputPorts)),
      outPorts_(std::move(outputPorts)) {}

float InputPort::getSample() { return sample_; }

void InputPort::setSample(float value) { sample_ = value; }

void OutputPort::connect(InputPort& destination) {
    destination_ = std::ref(destination);
}

void OutputPort::setSample(float value) {
    if (destination_.has_value()) {
        destination_->get().setSample(value);
    };
}

const OutputPort::DestinationPort OutputPort::viewDestination() const {
    return destination_;
}

} // namespace blocks