#include "process_block.h"

namespace blocks {

ProcessBlock::ProcessBlock(Process& process)
    : Block({InputPort(*this)}, {OutputPort(*this)}), process_(process) {}

void ProcessBlock::evaluate() {
    float sample = getInputPorts()[0].getSample();
    getOutputPorts()[0].setSample(process_.process(sample));
}

} // namespace blocks
