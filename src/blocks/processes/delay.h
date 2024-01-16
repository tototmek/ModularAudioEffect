#ifndef BLOCKS_PROCESSES_DELAY_H
#define BLOCKS_PROCESSES_DELAY_H

#include "process.h"
#include "shift_register.h"

namespace blocks {

class Delay : public Process {
  public:
    Delay(float time);
    float process(float x) override;

  private:
    size_t nSamples_;
    ShiftRegister<float> register_;
};

} // namespace blocks

#endif // BLOCKS_PROCESSES_DELAY_H
