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
          const std::vector<OutputPort>& outputPorts);
    Block(std::vector<InputPort>&& inputPorts,
          std::vector<OutputPort>&& outputPorts);
    uint32_t getId() const { return id_; };
    std::vector<InputPort>& getInputPorts() { return inPorts_; }
    std::vector<OutputPort>& getOutputPorts() { return outPorts_; }
    const std::vector<InputPort>& viewInputPorts() const { return inPorts_; }
    const std::vector<OutputPort>& viewOutputPorts() const { return outPorts_; }
    virtual void evaluate() = 0;

  private:
    static uint32_t nextId_;
    uint32_t id_;
    std::vector<InputPort> inPorts_;
    std::vector<OutputPort> outPorts_;
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
    float getSample();
    void setSample(float value);

  private:
    float sample_ = 0.0f;
};

class OutputPort : public Port {
  public:
    OutputPort(Block& parent) : Port(parent) {}
    void connect(InputPort& destination);
    void setSample(float value);
    using DestinationPort = std::optional<std::reference_wrapper<InputPort>>;
    const DestinationPort viewDestination() const;

  private:
    DestinationPort destination_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_H