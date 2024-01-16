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