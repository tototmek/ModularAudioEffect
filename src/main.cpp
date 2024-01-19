#include "audio/audio_client_portaudio.h"
#include "blocks/blocks.h"
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

    float dry = 0.6f;
    float wet = 0.4f;
    float feedback = 0.3f;
    float time = 0.8f;
    auto dryGain = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(dry));
    auto wetGain = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(wet));
    auto feedbackGain = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Gain>(feedback));
    auto delay = std::make_shared<blocks::ProcessBlock>(
        std::make_unique<blocks::Delay>(time));
    auto splitter1 = std::make_shared<blocks::Splitter>(2);
    auto splitter2 = std::make_shared<blocks::Splitter>(2);
    auto adder1 = std::make_shared<blocks::Adder>(2);
    auto adder2 = std::make_shared<blocks::Adder>(2);
    auto effect = std::make_shared<blocks::BlockSystem>();
    dryGain->setName("dryGain");
    wetGain->setName("wetGain");
    feedbackGain->setName("feedbackGain");
    delay->setName("delay");
    splitter1->setName("splitter1");
    splitter2->setName("splitter2");
    adder1->setName("adder1");
    adder2->setName("adder2");
    effect->setName("effect");
    effect->addBlock(dryGain);
    effect->addBlock(wetGain);
    effect->addBlock(feedbackGain);
    effect->addBlock(delay);
    effect->addBlock(adder1);
    effect->addBlock(adder2);
    effect->addBlock(splitter1);
    effect->addBlock(splitter2);
    i = 0;
    blocks::Connection connection;
    connection.source.block = splitter1;
    connection.source.port = 0;
    connection.target.block = dryGain;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = splitter1;
    connection.source.port = 1;
    connection.target.block = adder1;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = dryGain;
    connection.source.port = 0;
    connection.target.block = adder2;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = adder1;
    connection.source.port = 0;
    connection.target.block = delay;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = delay;
    connection.source.port = 0;
    connection.target.block = splitter2;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = splitter2;
    connection.source.port = 0;
    connection.target.block = wetGain;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = wetGain;
    connection.source.port = 0;
    connection.target.block = adder2;
    connection.target.port = 1;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = splitter2;
    connection.source.port = 1;
    connection.target.block = feedbackGain;
    connection.target.port = 0;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    connection.source.block = feedbackGain;
    connection.source.port = 0;
    connection.target.block = adder1;
    connection.target.port = 1;
    spdlog::info("Connecting {}...", i++);
    effect->addConnection(connection);
    blocks::Port port;
    port.block = splitter1;
    port.port = 0;
    effect->addInput(port);
    port.block = adder2;
    port.port = 0;
    effect->addOutput(port);

    client.setCallback(
        [&](const std::vector<float>& in, std::vector<float>& out) {
            effect->setInput(FUZZ(in[1]));
            effect->evaluate();
            float sample = effect->getOutput();
            out[1] = sample;
            out[0] = sample;
        });
    client.startStream();
    while (true) {
    }
}
