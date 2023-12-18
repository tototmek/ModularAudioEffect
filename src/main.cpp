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

    gainBlock.getOutputPort().connect(delayBlock.getInputPort());
    delayBlock.getOutputPort().connect(sinkBlock.getInputPort());

    const auto processData = [&](float sample) -> float {
        gainBlock.getInputPort().setSample(sample);
        gainBlock.evaluate();
        delayBlock.evaluate();
        return sinkBlock.getInputPort().getSample();
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