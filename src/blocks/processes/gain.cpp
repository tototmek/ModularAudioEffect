#include "gain.h"

namespace blocks {

Gain::Gain(float gain) : gain_(gain) {}

float Gain::process(float x) { return x * gain_; }

} // namespace blocks