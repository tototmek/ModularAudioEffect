#ifndef BLOCKS_GAIN_H
#define BLOCKS_GAIN_H

#include "process.h"

namespace blocks {

class Gain : public Process {
  public:
    Gain(float gain);
    float process(float x) override;

  private:
    float gain_;
};

} // namespace blocks

#endif // BLOCKS_GAIN_H