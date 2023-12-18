#include "single_process_block.h"

namespace blocks {

SingleProcessBlock::SingleProcessBlock(Process& process)
    : inputPort_(*this), outputPort_(*this), process_(process) {}

void SingleProcessBlock::evaluate() {
    float sample = inputPort_.getSample();
    outputPort_.setSample(process_.process(sample));
}

} // namespace blocks
