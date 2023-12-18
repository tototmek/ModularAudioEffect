#include "composite_block.h"

namespace blocks {

CompositeBlock::CompositeBlock(
    size_t nInPorts, size_t nOutPorts,
    std::vector<std::reference_wrapper<Block>> components)
    : inputPorts_(nInPorts, InputPort(*this)),
      outputPorts_(nOutPorts, OutputPort(*this)), components_(components) {}

void CompositeBlock::evaluate() {
    for (const auto& inputPort : inputPorts_) {
        (void)inputPort;
    }
}

} // namespace blocks
