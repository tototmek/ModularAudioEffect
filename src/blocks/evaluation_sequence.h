#ifndef BLOCKS_EVALUATION_SEQUENCE_H
#define BLOCKS_EVALUATION_SEQUENCE_H

#include "block_system.h"
#include <map>
#include <vector>

namespace blocks {

using graph_t = std::map<uint, std::vector<uint>>;

using Blocks_t = std::vector<std::shared_ptr<Block>>;
using Connections_t = std::map<std::shared_ptr<Block>, std::vector<Connection>>;

std::vector<uint> computeEvaluationSequence(graph_t graph);
std::vector<uint> computeEvaluationSequence(const Blocks_t& blocks,
                                            const Connections_t& connections);

} // namespace blocks

#endif // BLOCKS_EVALUATION_SEQUENCE_H
