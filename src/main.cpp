#include "blocks/core.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);

    blocks::Gain gain{2.0f};
    blocks::SingleProcessBlock gainBlock{gain};

    blocks::Delay delay(3.0f / blocks::kSampleRate);
    blocks::SingleProcessBlock delayBlock{delay};

    blocks::Gain sink{1.0f};
    blocks::SingleProcessBlock sinkBlock{sink};

    blocks::Splitter splitter{2};
    blocks::Adder adder{2};

    gainBlock.getOutputPorts()[0].connect(delayBlock.getInputPorts()[0]);
    delayBlock.getOutputPorts()[0].connect(splitter.getInputPorts()[0]);
    splitter.getOutputPorts()[0].connect(adder.getInputPorts()[0]);
    splitter.getOutputPorts()[1].connect(adder.getInputPorts()[1]);
    adder.getOutputPorts()[0].connect(sinkBlock.getInputPorts()[0]);

    blocks::EvaluationSequence evaluationSequence;
    evaluationSequence.compute(
        {gainBlock, delayBlock, splitter, adder, sinkBlock});

    const auto processData = [&](float sample) -> float {
        // evaluationSequence.evaluate();
        gainBlock.getInputPorts()[0].setSample(sample);
        gainBlock.evaluate();
        delayBlock.evaluate();
        splitter.evaluate();
        adder.evaluate();
        return sinkBlock.getInputPorts()[0].getSample();
    };

    spdlog::info("Delay test");
    float input = 1.0f;
    float output = processData(input);
    spdlog::info("{} -> {}", input, output);
    input = 2.0f;
    output = processData(input);
    spdlog::info("{} -> {}", input, output);
    input = 3.0f;
    output = processData(input);
    spdlog::info("{} -> {}", input, output);
    for (size_t i = 0; i < 10; ++i) {
        input = 0.0f;
        output = processData(input);
        spdlog::info("{} -> {}", input, output);
    }

    return 0;
}