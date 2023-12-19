#include "blocks/core.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);

    blocks::Gain gain{0.8f};
    blocks::SingleProcessBlock gain1Block{gain};
    blocks::SingleProcessBlock gain2Block{gain};
    blocks::SingleProcessBlock gain3Block{gain};
    blocks::SingleProcessBlock gain4Block{gain};
    blocks::SingleProcessBlock gain5Block{gain};

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
    evaluationSequence.compute({gain1Block, splitter1, gain2Block, adder1,
                                gain3Block, adder2, delayBlock, splitter2,
                                gain4Block, gain5Block});

    // const auto processData = [&](float sample) -> float {
    //     // evaluationSequence.evaluate();
    //     // gainBlock.getInputPorts()[0].setSample(sample);
    //     // gainBlock.evaluate();
    //     // delayBlock.evaluate();
    //     // splitter.evaluate();
    //     // adder.evaluate();
    //     // return sinkBlock.getInputPorts()[0].getSample();
    //     (void)sample;
    //     return 0.0f;
    // };

    // spdlog::info("Delay test");
    // float input = 1.0f;
    // float output = processData(input);
    // spdlog::info("{} -> {}", input, output);
    // input = 2.0f;
    // output = processData(input);
    // spdlog::info("{} -> {}", input, output);
    // input = 3.0f;
    // output = processData(input);
    // spdlog::info("{} -> {}", input, output);
    // for (size_t i = 0; i < 10; ++i) {
    //     input = 0.0f;
    //     output = processData(input);
    //     spdlog::info("{} -> {}", input, output);
    // }

    return 0;
}