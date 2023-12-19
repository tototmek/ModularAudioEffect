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
    Block(const std::vector<InputPort>& inputPorts,
          const std::vector<OutputPort>& outputPorts)
        : id_(nextId_++), inputPorts_(inputPorts), outputPorts_(outputPorts){};
    Block(std::vector<InputPort>&& inputPorts,
          std::vector<OutputPort>&& outputPorts)
        : id_(nextId_++), inputPorts_(std::move(inputPorts)),
          outputPorts_(std::move(outputPorts)){};
    uint32_t getId() const { return id_; };
    std::vector<InputPort>& getInputPorts() { return inputPorts_; }
    std::vector<OutputPort>& getOutputPorts() { return outputPorts_; }
    const std::vector<InputPort>& viewInputPorts() const { return inputPorts_; }
    const std::vector<OutputPort>& viewOutputPorts() const {
        return outputPorts_;
    }
    virtual void evaluate() = 0;

  private:
    static uint32_t nextId_;
    uint32_t id_;
    std::vector<InputPort> inputPorts_;
    std::vector<OutputPort> outputPorts_;
};

class Port {
  public:
    Port(Block& parent) : parent_(parent) {}
    Block& getParent() { return parent_; }
    const Block& viewParent() { return parent_; }

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
    const std::optional<std::reference_wrapper<InputPort>>
    viewDestination() const {
        return destination_;
    }

  private:
    std::optional<std::reference_wrapper<InputPort>> destination_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_H