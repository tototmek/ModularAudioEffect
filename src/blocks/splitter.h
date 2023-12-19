#ifndef BLOCKS_SPLITTER_H
#define BLOCKS_SPLITTER_H

#include "block.h"
#include <vector>

namespace blocks {

class Splitter : public Block {
  public:
    Splitter(size_t nOutputs);
    void evaluate() override;
};

} // namespace blocks

#endif // BLOCKS_SPLITTER_H
