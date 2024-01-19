#include "adder.h"
#include <numeric>

namespace blocks {

Adder::Adder(uint nInputs) : BlockAtomic(nInputs, 1) {}

void Adder::evaluate() {
    outputs_[0] = std::accumulate(inputs_.cbegin(), inputs_.cend(), 0.0f);
}

} // namespace blocks
