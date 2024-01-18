#include "audio/audio_client_portaudio.h"
#include <cmath>
#include <memory>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

float CubicAmplifier(float input) {
    float output, temp;
    if (input < 0.0) {
        temp = input + 1.0f;
        output = (temp * temp * temp) - 1.0f;
    } else {
        temp = input - 1.0f;
        output = (temp * temp * temp) + 1.0f;
    }
    return output;
}
#define FUZZ(x)                                                                \
    CubicAmplifier(CubicAmplifier(CubicAmplifier(CubicAmplifier(x))))

int main() {
    audio::PortAudioClient client;
    auto inputDevices = client.getAvailableInputDevices();
    auto outputDevices = client.getAvailableOutputDevices();
    uint i = 0;
    spdlog::info("Available input devices:");
    for (const auto& device : inputDevices) {
        spdlog::info("{:>3}. {}", i++, device.name);
    }
    i = 0;
    spdlog::info("Available output devices:");
    for (const auto& device : outputDevices) {
        spdlog::info("{:>3}. {}", i++, device.name);
    }
    client.setInputDevice(inputDevices[3]);
    client.setOutputDevice(outputDevices[8]);
    client.setCallback(
        [](const std::vector<float>& in, std::vector<float>& out) {
            float sample = FUZZ(in[1]);
            out[1] = sample;
            out[0] = sample;
        });
    client.startStream();
    while (true) {
    }
}
