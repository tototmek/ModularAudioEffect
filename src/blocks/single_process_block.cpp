#include "single_process_block.h"

namespace blocks {

SingleProcessBlock::SingleProcessBlock(Process& process)
    : Block({InputPort(*this)}, {OutputPort(*this)}), process_(process) {}

void SingleProcessBlock::evaluate() {
    float sample = getInputPorts()[0].getSample();
    getOutputPorts()[0].setSample(process_.process(sample));
}

} // namespace blocks
