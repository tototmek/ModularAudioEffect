#ifndef BLOCKS_BLOCK_H
#define BLOCKS_BLOCK_H

#include <functional>
#include <optional>

namespace blocks {

class InputPort;
class OutputPort;

class Block {
  public:
    virtual void evaluate() = 0;
};

class InputPort {
  public:
    float getSample() { return sample_; };
    void setSample(float value) { sample_ = value; };

  private:
    float sample_ = 0.0f;
};

class OutputPort {
  public:
    void connect(InputPort& destination) {
        destination_ = std::ref(destination);
    };
    void setSample(float value) {
        if (destination_.has_value()) {
            destination_->get().setSample(value);
        };
    }

  private:
    std::optional<std::reference_wrapper<InputPort>> destination_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_H