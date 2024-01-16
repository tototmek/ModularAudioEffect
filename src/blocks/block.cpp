#include "block.h"
#include "exceptions.h"
#include <algorithm>
#include <spdlog/fmt/fmt.h>

namespace blocks {

Block::Block(uint nInputs, uint nOutputs)
    : inputs_(nInputs, 0), outputs_(nOutputs, 0) {}

void Block::setInput(float value, uint portIdx) {
    if (portIdx >= inputs_.size()) {
        throw illegal_port_error(
            fmt::format("Requested block input with index '{}' out of bounds "
                        "(total input ports: {})",
                        portIdx, inputs_.size()));
    }
    inputs_[portIdx] = value;
}

float Block::getOutput(uint portIdx) const {
    if (portIdx >= outputs_.size()) {
        throw illegal_port_error(
            fmt::format("Requested block output with index '{}' out of bounds "
                        "(total output ports: {})",
                        portIdx, outputs_.size()));
    }
    return outputs_[portIdx];
}

uint Block::getInputSize() const { return inputs_.size(); }
uint Block::getOutputSize() const { return outputs_.size(); }

BlockAtomic::BlockAtomic(uint nInputs, uint nOutputs)
    : Block(nInputs, nOutputs) {}

BlockComposite::BlockComposite(uint nInputs, uint nOutputs)
    : Block(nInputs, nOutputs) {}

void BlockComposite::addBlock(std::shared_ptr<Block> block) {
    if (std::find(blocks_.cbegin(), blocks_.cend(), block) != blocks_.cend()) {
        throw invalid_operation_error(
            "Given block already present in the block system");
    }
    blocks_.emplace_back(block);
}

void BlockComposite::removeBlock(std::shared_ptr<Block> block) {
    auto it = std::find(blocks_.cbegin(), blocks_.cend(), block);
    if (it == blocks_.cend()) {
        throw invalid_operation_error(
            "Requested block not present in the block system");
    }
    blocks_.erase(it);
}

} // namespace blocks