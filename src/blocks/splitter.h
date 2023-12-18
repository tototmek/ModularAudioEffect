#ifndef BLOCKS_SPLITTER_H
#define BLOCKS_SPLITTER_H

#include "block.h"
#include <vector>

namespace blocks {

class Splitter : public Block {
  public:
    Splitter(size_t nOutputs);
    void evaluate() override;
    InputPort& getInputPort() { return inputPort_; }
    std::vector<OutputPort>& getOutputPorts() { return outputPorts_; }

  private:
    InputPort inputPort_;
    std::vector<OutputPort> outputPorts_;
};

} // namespace blocks

#endif // BLOCKS_SPLITTER_H
