#ifndef BLOCKS_PROCESSES_PROCESS_H
#define BLOCKS_PROCESSES_PROCESS_H

#include <cstddef>

namespace blocks {

constexpr size_t kMaxBufferSize = 131072;
constexpr int kSampleRate = 44100;

/*
Basic building block of a processing pipeline. Represents a single process –
with one input and one output. Various implementations can perform different
functions.
*/
class Process {
  public:
    virtual float process(float x) = 0;
};

} // namespace blocks

#endif // BLOCKS_PROCESSES_PROCESS_H