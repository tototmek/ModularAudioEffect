#ifndef BLOCKS_BLOCK_SYSTEM_H
#define BLOCKS_BLOCK_SYSTEM_H

#include "block.h"
#include <map>
#include <memory>
#include <vector>

namespace blocks {

struct Port {
    std::shared_ptr<Block> block;
    uint port = 0;
    bool operator==(const Port& rhs) const {
        return (block == rhs.block && port == rhs.port);
    }
};

struct Connection {
    Port source;
    Port target;
    bool operator==(const Connection& rhs) const {
        return (source == rhs.source && target == rhs.target);
    }
};

class BlockSystem : public BlockComposite {
  public:
    BlockSystem();
    void evaluate() override;
    void addConnection(Connection connection);
    void removeConnection(Connection connection);
    void addInput(Port port);
    void removeInput(Port port);
    void addOutput(Port port);
    void removeOutput(Port port);
    void updateEvaluationSequence();
    const std::vector<uint>& viewEvaluationSequence() { return evalSequence_; }
    bool hasBlock(std::shared_ptr<Block> block) const;
    bool hasConnection(Connection connection) const;

  private:
    enum class PortType { INPUT, OUTPUT };
    bool isPortConnected(Port port, PortType type) const;
    bool shouldUpdateEvalSequence_ = false;
    std::vector<uint> evalSequence_;
    std::map<std::shared_ptr<Block>, std::vector<Connection>> connections_;
    std::vector<Port> inputConnections_;
    std::vector<Port> outputConnections_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_SYSTEM_H
