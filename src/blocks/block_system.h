#ifndef BLOCKS_BLOCK_SYSTEM_H
#define BLOCKS_BLOCK_SYSTEM_H

#include "block.h"
#include <map>
#include <memory>
#include <vector>

namespace blocks {

struct Port {
    std::shared_ptr<Block> block;
    uint port;
    bool operator==(const Port& rhs) {
        return (block == rhs.block && port == rhs.port);
    }
};

struct Connection {
    Port source;
    Port target;
    bool operator==(const Connection& rhs) {
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

  private:
    bool shouldUpdateEvalSequence_ = false;
    std::vector<uint> evalSequence_;
    std::map<std::shared_ptr<Block>, std::vector<Connection>> connections_;
    std::vector<Port> inputConnections_;
    std::vector<Port> outputConnections_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_SYSTEM_H
