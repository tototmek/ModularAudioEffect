#include "delay.h"

namespace blocks {

Delay::Delay(float time)
    : nSamples_(size_t(kSampleRate * time)), register_(kMaxBufferSize) {}

float Delay::process(float x) {
    register_.push(x);
    return register_.at(nSamples_);
}

} // namespace blocks
