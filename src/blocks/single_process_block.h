#ifndef BLOCKS_SINGLE_PROCESS_BLOCK_H
#define BLOCKS_SINGLE_PROCESS_BLOCK_H

#include "block.h"
#include "processes/process.h"

namespace blocks {

class SingleProcessBlock : public Block {
  public:
    SingleProcessBlock(Process& process);
    void evaluate() override;

  private:
    Process& process_;
};

} // namespace blocks

#endif // BLOCKS_SINGLE_PROCESS_BLOCK_H
