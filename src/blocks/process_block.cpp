#include "process_block.h"

namespace blocks {

ProcessBlock::ProcessBlock(std::unique_ptr<Process> process)
    : BlockAtomic(1, 1), process_(std::move(process)) {}

void ProcessBlock::evaluate() {
    float sample = inputs_[0];
    outputs_[0] = process_->process(sample);
}

} // namespace blocks
