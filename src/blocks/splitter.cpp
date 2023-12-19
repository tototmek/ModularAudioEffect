#include "splitter.h"

namespace blocks {

Splitter::Splitter(size_t nOutputs)
    : Block({InputPort(*this)},
            std::vector<OutputPort>(nOutputs, OutputPort(*this))) {}

void Splitter::evaluate() {
    for (auto& outputPort : getOutputPorts()) {
        outputPort.setSample(getInputPorts()[0].getSample());
    }
}

} // namespace blocks
