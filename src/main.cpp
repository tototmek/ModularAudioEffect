#include "audio/audio_client_portaudio.h"
#include <memory>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

int main() {
    audio::PortAudioClient client;
    auto inputDevices = client.getInputDevices();
    spdlog::info("Available input devices:");
    for (const auto& device : inputDevices) {
        spdlog::info("{}. {:<50}\tChannels: {} in, {} out"
                     "\tSupported sample rates: [{}]",
                     device.index, device.name, device.inputChannels,
                     device.outputChannels,
                     fmt::join(device.sampleRates, ", "));
    }
    auto outputDevices = client.getOutputDevices();
    spdlog::info("Available output devices:");
    for (const auto& device : outputDevices) {
        spdlog::info("{}. {:<50}\tChannels: {} in, {} out\t"
                     "\tSupported sample rates: [{}]",
                     device.index, device.name, device.inputChannels,
                     device.outputChannels,
                     fmt::join(device.sampleRates, ", "));
    }
    client.setInputDevice(inputDevices[3]);
    client.setOutputDevice(outputDevices[8]);
    Pa_Sleep(10000);
}
