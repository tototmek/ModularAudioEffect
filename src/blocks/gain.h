#ifndef BLOCKS_GAIN_H
#define BLOCKS_GAIN_H

#include "processing_block.h"

namespace blocks {

class Gain : public ProcessingBlock {
  public:
    Gain(float gain);
    float process(float x) override;

  private:
    float gain_;
};

} // namespace blocks

#endif // BLOCKS_GAIN_H