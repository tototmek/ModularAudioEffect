#ifndef BLOCKS_BLOCK_H
#define BLOCKS_BLOCK_H

#include <memory>
#include <vector>

namespace blocks {

using PortValues_t = std::vector<float>;

class Block {
  public:
    Block(uint nInputs, uint nOutputs);
    virtual void evaluate() = 0;
    void setInput(float value, uint portIdx = 0);
    float getOutput(uint portIdx = 0);

  protected:
    PortValues_t inputs_;
    PortValues_t outputs_;
};

class BlockAtomic : public Block {
  public:
    BlockAtomic(uint nInputs, uint nOutputs);
    virtual void evaluate() override = 0;
};

class BlockComposite : public Block {
  public:
    BlockComposite(uint nInputs, uint nOutputs);
    virtual void evaluate() override = 0;
    void addBlock(std::shared_ptr<Block> block);
    void removeBlock(std::shared_ptr<Block> block);

  protected:
    std::vector<std::shared_ptr<Block>> blocks_;
};

} // namespace blocks

#endif // BLOCKS_BLOCK_H