#ifndef BLOCKS_ADDER_H
#define BLOCKS_ADDER_H

#include "block.h"
#include <vector>

namespace blocks {

class Adder : public Block {
  public:
    Adder(size_t nInputs);
    void evaluate() override;
};

} // namespace blocks

#endif // BLOCKS_ADDER_H
