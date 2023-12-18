#include "splitter.h"

namespace blocks {

Splitter::Splitter(size_t nOutputs)
    : inputPort_(*this), outputPorts_(nOutputs, OutputPort(*this)) {}

void Splitter::evaluate() {
    for (auto& outputPort : outputPorts_) {
        outputPort.setSample(inputPort_.getSample());
    }
}

} // namespace blocks
