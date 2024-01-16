#include "block.h"
#include <algorithm>

namespace blocks {

Block::Block(uint nInputs, uint nOutputs)
    : inputs_(nInputs, 0), outputs_(nOutputs, 0) {}

void Block::setInput(float value, uint portIdx) { inputs_[portIdx] = value; }

float Block::getOutput(uint portIdx) { return outputs_[portIdx]; }

BlockAtomic::BlockAtomic(uint nInputs, uint nOutputs)
    : Block(nInputs, nOutputs) {}

BlockComposite::BlockComposite(uint nInputs, uint nOutputs)
    : Block(nInputs, nOutputs) {}

void BlockComposite::addBlock(std::shared_ptr<Block> block) {
    blocks_.emplace_back(block);
}

void BlockComposite::removeBlock(std::shared_ptr<Block> block) {
    std::remove(blocks_.begin(), blocks_.end(), block);
}

} // namespace blocks