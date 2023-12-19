#include "blocks/core.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);

    blocks::Gain gain{2.0f};
    blocks::SingleProcessBlock gain1Block{gain};
    blocks::SingleProcessBlock gain2Block{gain};
    blocks::SingleProcessBlock gain3Block{gain};
    blocks::SingleProcessBlock gain4Block{gain};
    blocks::Gain attenuation{1.0f / 3.0f};
    blocks::SingleProcessBlock gain5Block{attenuation};

    blocks::Delay delay(3.0f / blocks::kSampleRate);
    blocks::SingleProcessBlock delayBlock{delay};

    blocks::Splitter splitter1{2};
    blocks::Splitter splitter2{2};
    blocks::Adder adder1{2};
    blocks::Adder adder2{2};

    gain1Block.getOutputPorts()[0].connect(splitter1.getInputPorts()[0]);
    splitter1.getOutputPorts()[0].connect(gain2Block.getInputPorts()[0]);
    splitter1.getOutputPorts()[1].connect(adder2.getInputPorts()[0]);
    gain2Block.getOutputPorts()[0].connect(adder1.getInputPorts()[0]);
    adder1.getOutputPorts()[0].connect(gain3Block.getInputPorts()[0]);
    adder2.getOutputPorts()[0].connect(delayBlock.getInputPorts()[0]);
    delayBlock.getOutputPorts()[0].connect(splitter2.getInputPorts()[0]);
    splitter2.getOutputPorts()[0].connect(gain4Block.getInputPorts()[0]);
    splitter2.getOutputPorts()[1].connect(gain5Block.getInputPorts()[0]);
    gain4Block.getOutputPorts()[0].connect(adder1.getInputPorts()[1]);
    gain5Block.getOutputPorts()[0].connect(adder2.getInputPorts()[1]);

    blocks::EvaluationSequence evaluationSequence;
    evaluationSequence.compute({gain2Block, splitter1, gain1Block, adder2,
                                gain3Block, adder1, delayBlock, splitter2,
                                gain5Block, gain4Block});

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
