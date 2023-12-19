#include "adder.h"

namespace blocks {

Adder::Adder(size_t nInputs)
    : Block(std::vector<InputPort>(nInputs, InputPort(*this)),
            {OutputPort(*this)}) {}

void Adder::evaluate() {
    float value = 0.0f;
    for (auto& inputPort : getInputPorts()) {
        value += inputPort.getSample();
    }
    getOutputPorts()[0].setSample(value);
}

} // namespace blocks
