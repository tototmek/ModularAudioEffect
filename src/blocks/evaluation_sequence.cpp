#include "evaluation_sequence.h"

namespace blocks {

void EvaluationSequence::compute(
    const std::vector<std::reference_wrapper<InputPort>>& inputs) {
    // Go along all signal routes and add blocks to sequence
    (void)inputs;
}

void EvaluationSequence::evaluate() {
    for (auto& block : blockSequence_) {
        block.get().evaluate();
    }
}

} // namespace blocks
