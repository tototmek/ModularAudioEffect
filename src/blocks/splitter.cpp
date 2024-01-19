#include "splitter.h"

namespace blocks {

Splitter::Splitter(uint nOutputs) : BlockAtomic(1, nOutputs) {}

void Splitter::evaluate() {
    for (auto& output : outputs_) {
        output = inputs_[0];
    }
}

} // namespace blocks
