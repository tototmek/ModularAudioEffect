#ifndef BLOCKS_SINGLE_PROCESS_BLOCK_H
#define BLOCKS_SINGLE_PROCESS_BLOCK_H

#include "block.h"
#include "processes/process.h"

namespace blocks {

class SingleProcessBlock : public Block {
  public:
    SingleProcessBlock(Process& process);
    void evaluate() override;
    InputPort& getInputPort() { return inputPort_; }
    OutputPort& getOutputPort() { return outputPort_; }

  private:
    InputPort inputPort_;
    OutputPort outputPort_;
    Process& process_;
};

} // namespace blocks

#endif // BLOCKS_SINGLE_PROCESS_BLOCK_H
