#ifndef BLOCKS_ADDER_H
#define BLOCKS_ADDER_H

#include "block.h"
#include <vector>

namespace blocks {

class Adder : public Block {
  public:
    Adder(size_t nInputs);
    void evaluate() override;
    OutputPort& getOutputPort() { return outputPort_; }
    std::vector<InputPort>& getInputPorts() { return inputPorts_; }

  private:
    OutputPort outputPort_;
    std::vector<InputPort> inputPorts_;
};

} // namespace blocks

#endif // BLOCKS_ADDER_H
