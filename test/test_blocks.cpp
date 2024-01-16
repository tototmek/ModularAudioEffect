#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <spdlog/spdlog.h>

#include <../src/blocks/blocks.h>

TEST_CASE("Always pass") { ; }

TEST_CASE("Construct process block") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
}

TEST_CASE("Evaluate process block") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    block->setInput(1.0f);
    block->evaluate();
    REQUIRE_THAT(block->getOutput(), Catch::Matchers::WithinAbs(1.0f, 1e-4f));
}

TEST_CASE("Get input out of bounds") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    REQUIRE_THROWS_AS(block->setInput(1.0f, 1), blocks::base_exception);
}

TEST_CASE("Get output out of bounds") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    REQUIRE_THROWS_AS(block->getOutput(1), blocks::base_exception);
}

TEST_CASE("Construct block system") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
}

TEST_CASE("Add block") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block);
}

TEST_CASE("Remove block") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block);
    blockSystem->removeBlock(block);
}

TEST_CASE("Add same block twice") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block);
    REQUIRE_THROWS_AS(blockSystem->addBlock(block), blocks::base_exception);
}

TEST_CASE("Remove non-existent block") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    REQUIRE_THROWS_AS(blockSystem->removeBlock(block), blocks::base_exception);
}

TEST_CASE("hasBlock() method") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    CHECK(blockSystem->hasBlock(block) == false);
    blockSystem->addBlock(block);
    CHECK(blockSystem->hasBlock(block) == true);
    blockSystem->removeBlock(block);
    REQUIRE(blockSystem->hasBlock(block) == false);
}

TEST_CASE("hasBlock() method with more blocks") {
    auto block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto unusedBlock = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(unusedBlock);
    CHECK(blockSystem->hasBlock(block) == false);
    blockSystem->addBlock(block);
    CHECK(blockSystem->hasBlock(block) == true);
    blockSystem->removeBlock(block);
    REQUIRE(blockSystem->hasBlock(block) == false);
}

TEST_CASE("Add connection") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    blockSystem->addConnection(connection);
}

TEST_CASE("hasConnection() method") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.source.port = 0;
    connection.target.block = blockB;
    connection.target.port = 0;
    CHECK(blockSystem->hasConnection(connection) == false);
    blockSystem->addConnection(connection);
    CHECK(blockSystem->hasConnection(connection) == true);
    blockSystem->removeConnection(connection);
    REQUIRE(blockSystem->hasConnection(connection) == false);
}

TEST_CASE("Add connection of blocks not inside block system") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection from outside") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection to outside") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection illegal source port") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.source.port = 1;
    connection.target.block = blockB;
    connection.target.port = 0;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection illegal target port") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.source.port = 0;
    connection.target.block = blockB;
    connection.target.port = 1;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add duplicate connection") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.source.port = 0;
    connection.target.block = blockB;
    connection.target.port = 0;
    blockSystem->addConnection(connection);
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection to already connected target port") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockC = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);
    blockSystem->addBlock(blockC);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.source.port = 0;
    connection.target.block = blockC;
    connection.target.port = 0;
    blockSystem->addConnection(connection);
    connection.source.block = blockB;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add connection from already connected target port") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockC = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);
    blockSystem->addBlock(blockC);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockC;
    blockSystem->addConnection(connection);
    connection.target.block = blockB;
    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add multiple valid connections") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockC = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);
    blockSystem->addBlock(blockC);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    blockSystem->addConnection(connection);
    connection.source.block = blockB;
    connection.target.block = blockC;
    blockSystem->addConnection(connection);
}

TEST_CASE("Remove connection") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    blockSystem->addConnection(connection);
    blockSystem->removeConnection(connection);
}

TEST_CASE("Remove connection which does not exist") {
    auto blockA = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockB = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockC = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(blockA);
    blockSystem->addBlock(blockB);
    blockSystem->addBlock(blockC);

    blocks::Connection connection;
    connection.source.block = blockA;
    connection.target.block = blockB;
    blockSystem->addConnection(connection);
    connection.source.block = blockB;
    connection.target.block = blockC;
    blockSystem->addConnection(connection);
    connection.source.block = blockA;
    connection.target.block = blockC;
    REQUIRE_THROWS_AS(blockSystem->removeConnection(connection),
                      blocks::base_exception);
}

bool compareVectors(const std::vector<uint>& a, const std::vector<uint>& b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            spdlog::error("Value {} should be equal {}", a[i], b[i]);
            return false;
        }
    }
    return true;
}

TEST_CASE("Compute evaluation sequence: straight graph") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blockSystem->updateEvaluationSequence();
    auto evalSequence = blockSystem->viewEvaluationSequence();
    std::vector<uint> correctEvalSequence{0, 1, 2};
    REQUIRE(compareVectors(evalSequence, correctEvalSequence));
}

TEST_CASE("Compute evaluation sequence: graph with cycle") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    connection.source.block = block2;
    connection.target.block = block0;
    blockSystem->addConnection(connection);
    blockSystem->updateEvaluationSequence();
    auto evalSequence = blockSystem->viewEvaluationSequence();
    std::vector<uint> correctEvalSequence{0, 1, 2};
    REQUIRE(compareVectors(evalSequence, correctEvalSequence));
}

TEST_CASE("Add input to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
}

TEST_CASE("Add multiple inputs to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block1;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addInput(port);
    REQUIRE(blockSystem->getInputSize() == 3);
}

TEST_CASE("Add input to block system, illegal port") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block0;
    port.port = 1;
    REQUIRE_THROWS_AS(blockSystem->addInput(port), blocks::base_exception);
}

TEST_CASE("Add input to block system, port already connected") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block1;
    port.port = 0;
    REQUIRE_THROWS_AS(blockSystem->addInput(port), blocks::base_exception);
}

TEST_CASE("Add output to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block2;
    blockSystem->addOutput(port);
}

TEST_CASE("Add multiple outputs to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Port port;
    port.block = block0;
    blockSystem->addOutput(port);
    port.block = block1;
    blockSystem->addOutput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    REQUIRE(blockSystem->getOutputSize() == 3);
}

TEST_CASE("Add output to block system, illegal port") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block2;
    port.port = 1;
    REQUIRE_THROWS_AS(blockSystem->addOutput(port), blocks::base_exception);
}

TEST_CASE("Add output to block system, port already connected") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block1;
    REQUIRE_THROWS_AS(blockSystem->addOutput(port), blocks::base_exception);
}

TEST_CASE("Remove input of block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    blockSystem->removeInput(port);
}

TEST_CASE("Remove non-existent input of block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block1;
    REQUIRE_THROWS_AS(blockSystem->removeInput(port), blocks::base_exception);
}

TEST_CASE("Remove output of block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->removeOutput(port);
}

TEST_CASE("Remove non-existent output of block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);

    blocks::Port port;
    port.block = block2;
    blockSystem->addOutput(port);
    port.block = block1;
    REQUIRE_THROWS_AS(blockSystem->removeOutput(port), blocks::base_exception);
}

TEST_CASE("Connect to a port that is already an input") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    connection.source.block = block2;
    connection.target.block = block0;

    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Connect to a port that is already an output") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);

    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block2;
    blockSystem->addOutput(port);
    connection.source.block = block2;
    connection.target.block = block0;

    REQUIRE_THROWS_AS(blockSystem->addConnection(connection),
                      blocks::base_exception);
}

TEST_CASE("Add and remove multiple outputs to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Port port;
    port.block = block0;
    blockSystem->addOutput(port);
    port.block = block1;
    blockSystem->addOutput(port);
    port.block = block0;
    blockSystem->removeOutput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    REQUIRE(blockSystem->getOutputSize() == 2);
}

TEST_CASE("Add and remove multiple inputs to block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block1;
    blockSystem->addInput(port);
    port.block = block0;
    blockSystem->removeInput(port);
    port.block = block2;
    blockSystem->addInput(port);
    REQUIRE(blockSystem->getInputSize() == 2);
}

TEST_CASE("Evaluate simple block system") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(64.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed connection") {
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeConnection(connection);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(0.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed block") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    {
        auto block0 = std::make_shared<blocks::ProcessBlock>(
            std::make_unique<blocks::Gain>(2.0));
        auto block1 = std::make_shared<blocks::ProcessBlock>(
            std::make_unique<blocks::Gain>(4.0));
        auto block2 = std::make_shared<blocks::ProcessBlock>(
            std::make_unique<blocks::Gain>(8.0));
        blockSystem->addBlock(block0);
        blockSystem->addBlock(block1);
        blockSystem->addBlock(block2);
        blocks::Connection connection;
        connection.source.block = block0;
        connection.target.block = block1;
        blockSystem->addConnection(connection);
        connection.source.block = block1;
        connection.target.block = block2;
        blockSystem->addConnection(connection);
        blocks::Port port;
        port.block = block0;
        blockSystem->addInput(port);
        port.block = block2;
        blockSystem->addOutput(port);
        blockSystem->setInput(1.0f);
        blockSystem->removeBlock(block1);
    }
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(0.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed block, reconnected") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeBlock(block1);
    connection.source.block = block0;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(16.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed input") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    port.block = block0;
    blockSystem->removeInput(port);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(0.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed output") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    port.block = block2;
    blockSystem->removeOutput(port);
    blockSystem->evaluate();
}

TEST_CASE("Evaluate simple block system removed input block") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeBlock(block1);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(0.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed output block") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeBlock(block2);
    blockSystem->evaluate();
}

TEST_CASE("Evaluate simple block system removed input block, reconnected") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeBlock(block0);
    port.block = block1;
    blockSystem->addInput(port);
    blockSystem->setInput(1.0f);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(32.0f, 1e-4f));
}

TEST_CASE("Evaluate simple block system removed output block, reconnected") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    connection.source.block = block1;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeBlock(block2);
    port.block = block1;
    blockSystem->addOutput(port);
    blockSystem->evaluate();
    REQUIRE_THAT(blockSystem->getOutput(),
                 Catch::Matchers::WithinAbs(8.0f, 1e-4f));
}

TEST_CASE("Reroute connection to another output") {
    auto blockSystem = std::make_shared<blocks::BlockSystem>();
    auto block0 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto block1 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto block2 = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(8.0));
    blockSystem->addBlock(block0);
    blockSystem->addBlock(block1);
    blockSystem->addBlock(block2);
    blocks::Connection connection;
    connection.source.block = block0;
    connection.target.block = block1;
    blockSystem->addConnection(connection);
    blocks::Port port;
    port.block = block0;
    blockSystem->addInput(port);
    port.block = block1;
    blockSystem->addOutput(port);
    port.block = block2;
    blockSystem->addOutput(port);
    blockSystem->setInput(1.0f);
    blockSystem->removeConnection(connection);
    connection.source.block = block0;
    connection.target.block = block2;
    blockSystem->addConnection(connection);
    blockSystem->evaluate();
    CHECK_THAT(blockSystem->getOutput(0),
               Catch::Matchers::WithinAbs(0.0f, 1e-4f));
    REQUIRE_THAT(blockSystem->getOutput(1),
                 Catch::Matchers::WithinAbs(16.0f, 1e-4f));
}