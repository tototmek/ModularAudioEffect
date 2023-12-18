#ifndef BLOCKS_BLOCK_H
#define BLOCKS_BLOCK_H

#include <functional>
#include <optional>

namespace blocks {

class Port;
class InputPort;
class OutputPort;

class Block {
  public:
    virtual void evaluate() = 0;
};

class Port {
  public:
    Port(Block& parent) : parent_(parent) {}
    Block& getParent() { return parent_; }

  private:
    Block& parent_;
};

class InputPort : public Port {
  public:
    InputPort(Block& parent) : Port(parent) {}
    float getSample() { return sample_; }
    void setSample(float value) { sample_ = value; }

  private:
    float sample_ = 0.0f;
};

class OutputPort : public Port {
  public:
    OutputPort(Block& parent) : Port(parent) {}
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