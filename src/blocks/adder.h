#ifndef BLOCKS_ADDER_H
#define BLOCKS_ADDER_H

#include "block.h"

namespace blocks {

class Adder : public BlockAtomic {
  public:
    Adder(uint nInputs);
    void evaluate() override;
};

} // namespace blocks

#endif // BLOCKS_ADDER_H
