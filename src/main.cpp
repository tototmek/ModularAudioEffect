#include "blocks/gain.h"
#include "project_info.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("{}", PROJECT_NAME);
    spdlog::info("Version {}", PROJECT_VERSION);
    blocks::Gain gain{2.0f};
    float input = 1.0f;
    float output = gain.process(input);
    spdlog::info("{} -> {}", input, output);
    return 0;
}