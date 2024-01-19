#ifndef BLOCKS_SPLITTER_H
#define BLOCKS_SPLITTER_H

#include "block.h"

namespace blocks {

class Splitter : public BlockAtomic {
  public:
    Splitter(uint nOutputs);
    void evaluate() override;
};

} // namespace blocks

#endif // BLOCKS_SPLITTER_H
