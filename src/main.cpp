#include "blocks/core.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);
    blocks::Gain gain{2.0f};
    float input = 1.0f;
    float output = gain.process(input);
    spdlog::info("Gain test");
    spdlog::info("{} -> {}", input, output);

    blocks::Delay delay(3.0f / blocks::kSampleRate);
    spdlog::info("Delay test");
    input = 1.0f;
    output = delay.process(input);
    spdlog::info("{} -> {}", input, output);
    input = 2.0f;
    output = delay.process(input);
    spdlog::info("{} -> {}", input, output);
    input = 3.0f;
    output = delay.process(input);
    spdlog::info("{} -> {}", input, output);
    for (size_t i = 0; i < 10; ++i) {
        input = 0.0f;
        output = delay.process(input);
        spdlog::info("{} -> {}", input, output);
    }

    return 0;
}