#ifndef BLOCKS_DELAY_H
#define BLOCKS_DELAY_H

#include "processing_block.h"
#include "shift_register.h"

namespace blocks {

class Delay : public ProcessingBlock {
  public:
    Delay(float time);
    float process(float x) override;

  private:
    ShiftRegister<float> register_;
};

} // namespace blocks

#endif // BLOCKS_DELAY_H
