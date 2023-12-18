#include "adder.h"

namespace blocks {

Adder::Adder(size_t nInputs)
    : outputPort_(*this), inputPorts_(nInputs, InputPort(*this)) {}

void Adder::evaluate() {
    float value = 0.0f;
    for (auto& inputPort : inputPorts_) {
        value += inputPort.getSample();
    }
    outputPort_.setSample(value);
}

} // namespace blocks
