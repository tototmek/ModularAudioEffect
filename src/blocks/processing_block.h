#ifndef BLOCKS_PROCESSING_BLOCK_H
#define BLOCKS_PROCESSING_BLOCK_H

#include <cstddef>

namespace blocks {

constexpr size_t kMaxBufferSize = 131072;
constexpr int kSampleRate = 44100;

class ProcessingBlock {
  public:
    virtual float process(float x) = 0;
};

} // namespace blocks

#endif // BLOCKS_PROCESSING_BLOCK_H