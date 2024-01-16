#ifndef BLOCKS_SINGLE_PROCESS_BLOCK_H
#define BLOCKS_SINGLE_PROCESS_BLOCK_H

#include "block.h"
#include "processes/process.h"

namespace blocks {

class ProcessBlock : public BlockAtomic {
  public:
    ProcessBlock(std::unique_ptr<Process> process);
    void evaluate() override;

  private:
    std::unique_ptr<Process> process_;
};

} // namespace blocks

#endif // BLOCKS_SINGLE_PROCESS_BLOCK_H
