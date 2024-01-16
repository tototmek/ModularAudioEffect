#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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
    blocks::Connection connection2;
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
