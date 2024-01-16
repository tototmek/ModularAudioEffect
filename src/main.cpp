#include "blocks/blocks.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);

    auto unusedGainBlock = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto gain1Block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(1.0));
    auto gain2Block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(2.0));
    auto gain3Block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(3.0));
    auto gain4Block = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(4.0));
    auto blockSystem = std::make_shared<blocks::BlockSystem>();

    blockSystem->addBlock(gain1Block);
    blockSystem->addBlock(gain2Block);
    blockSystem->addBlock(gain3Block);
    blockSystem->addBlock(gain4Block);

    blocks::Connection connection;
    connection.source.block = gain1Block;
    connection.source.port = 0;
    connection.target.block = gain2Block;
    connection.target.port = 0;
    blockSystem->addConnection(connection);
    connection.source.block = gain2Block;
    connection.source.port = 0;
    connection.target.block = gain3Block;
    connection.target.port = 0;
    blockSystem->addConnection(connection);
    connection.source.block = gain3Block;
    connection.source.port = 0;
    connection.target.block = gain4Block;
    connection.target.port = 0;
    blockSystem->addConnection(connection);

    blocks::Port in;
    in.block = gain1Block;
    in.port = 0;
    blockSystem->addInput(in);

    blocks::Port out;
    out.block = gain4Block;
    out.port = 0;
    blockSystem->addOutput(out);

    const auto processData = [&](float sample) -> float {
        blockSystem->setInput(sample);
        auto t1 = std::chrono::high_resolution_clock::now();
        blockSystem->evaluate();
        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        spdlog::debug("Graph evaluation took {} microseconds",
                      duration.count());

        return blockSystem->getOutput();
    };

    spdlog::info("Delay test");
    float input = 1.0f;
    float output = processData(input);
    spdlog::info("{} -> {}", input, output);
    for (size_t i = 0; i < 11; ++i) {
        input = 0.0f;
        output = processData(input);
        spdlog::info("{} -> {}", input, output);
    }

    return 0;
}
