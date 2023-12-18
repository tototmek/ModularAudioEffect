#ifndef BLOCKS_EVALUATION_SEQUENCE_H
#define BLOCKS_EVALUATION_SEQUENCE_H

#include "block.h"
#include <vector>

namespace blocks {

class EvaluationSequence {
  public:
    void compute(const std::vector<std::reference_wrapper<InputPort>>& inputs);
    void evaluate();

  private:
    std::vector<std::reference_wrapper<Block>> blockSequence_;
};

} // namespace blocks

#endif // BLOCKS_EVALUATION_SEQUENCE_H
