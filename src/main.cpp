#include "audio/audio_client_portaudio.h"
#include <memory>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

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
    client.startStream();
    Pa_Sleep(10000);
}
