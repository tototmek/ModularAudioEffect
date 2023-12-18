#ifndef BLOCKS_COMPOSITE_BLOCK_H
#define BLOCKS_COMPOSITE_BLOCK_H

#include "block.h"
#include <vector>

namespace blocks {

class CompositeBlock : public Block {
  public:
    CompositeBlock(size_t nInPorts, size_t nOutPorts,
                   std::vector<std::reference_wrapper<Block>> components);
    void evaluate() override;

  private:
    std::vector<InputPort> inputPorts_;
    std::vector<OutputPort> outputPorts_;
    std::vector<std::reference_wrapper<Block>> components_;
};

} // namespace blocks

#endif // BLOCKS_COMPOSITE_BLOCK_H
