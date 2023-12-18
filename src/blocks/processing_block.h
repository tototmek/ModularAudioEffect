#ifndef BLOCKS_PROCESSING_BLOCK_H
#define BLOCKS_PROCESSING_BLOCK_H

namespace blocks {

class ProcessingBlock {
  public:
    virtual float process(float x) = 0;
};

} // namespace blocks

#endif // BLOCKS_PROCESSING_BLOCK_H