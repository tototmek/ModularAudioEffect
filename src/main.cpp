#include "blocks/core.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);

    // Change the structure: blocks should have less responsibility: they should
    // be put in a container that manages their connections and the evaluation
    // order. Maybe use composite design pattern

    blocks::Gain gain{1.0f};
    blocks::ProcessBlock gain1Block{gain};
    blocks::Adder adder{2};
    blocks::ProcessBlock gain2Block{gain};
    blocks::Splitter splitter{2};
    blocks::ProcessBlock gain3Block{gain};
    blocks::ProcessBlock gain4Block{gain};

    gain1Block.getOutputPorts()[0].connect(adder.getInputPorts()[0]);
    adder.getOutputPorts()[0].connect(gain2Block.getInputPorts()[0]);
    gain2Block.getOutputPorts()[0].connect(splitter.getInputPorts()[0]);
    splitter.getOutputPorts()[0].connect(gain3Block.getInputPorts()[0]);
    splitter.getOutputPorts()[1].connect(gain4Block.getInputPorts()[0]);
    gain4Block.getOutputPorts()[0].connect(adder.getInputPorts()[1]);

    blocks::EvaluationSequence evaluationSequence;
    evaluationSequence.compute(
        {gain1Block, splitter, gain3Block, adder, gain4Block, gain2Block});

    const auto processData = [&](float sample) -> float {
        gain1Block.getInputPorts()[0].setSample(sample);

        auto t1 = std::chrono::high_resolution_clock::now();
        evaluationSequence.evaluate();
        auto t2 = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
        spdlog::debug("Graph evaluation took {} microseconds",
                      duration.count());

        return gain3Block.getInputPorts()[0].getSample();
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
