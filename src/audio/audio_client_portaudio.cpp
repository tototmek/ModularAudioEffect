#include "audio_client_portaudio.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace audio {

static const std::vector<double> kStandardSamplerates{
    22050.0, 24000.0, 44100.0, 48000.0, 88200.0, 96000.0, 176400.0, 192000.0};

namespace {

void checkError(PaError errorCode) {
    if (errorCode != paNoError) {
        throw std::runtime_error(
            fmt::format("PortAudio error: {}", Pa_GetErrorText(errorCode)));
    }
}

Device getDeviceAt(uint index) {
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
    Device device;
    device.index = index;
    device.name = std::string(deviceInfo->name);
    device.inputChannels = deviceInfo->maxInputChannels;
    device.outputChannels = deviceInfo->maxOutputChannels;
    device.inputLatency = deviceInfo->defaultLowInputLatency;
    device.outputLatency = deviceInfo->defaultLowOutputLatency;
    device.sampleRates = {};
    return device;
}

void updateDeviceSamplerates(Device& device, bool isInput) {
    device.sampleRates.clear();
    for (const auto& samplerate : kStandardSamplerates) {
        PaError result = paFormatIsSupported;
        PaStreamParameters params;
        params.device = device.index;
        params.hostApiSpecificStreamInfo = nullptr;
        params.sampleFormat = paFloat32;
        if (isInput) {
            params.channelCount = device.inputChannels;
            params.suggestedLatency = device.inputLatency;
            result = Pa_IsFormatSupported(&params, nullptr, samplerate);
        } else {
            params.channelCount = device.outputChannels;
            params.suggestedLatency = device.outputLatency;
            result = Pa_IsFormatSupported(nullptr, &params, samplerate);
        }
        if (result == paFormatIsSupported) {
            device.sampleRates.emplace_back(samplerate);
        }
    }
}

std::vector<Device> getAvailableDevices(bool isInput) {
    uint nDevices = Pa_GetDeviceCount();
    if (nDevices < 0) {
        throw std::runtime_error("PortAudio error: Pa_GetDeviceCount failed.");
    }
    std::vector<Device> devices;
    for (uint i = 0; i < nDevices; ++i) {
        Device device = getDeviceAt(i);
        if ((device.inputChannels == 0 && isInput) ||
            (device.outputChannels == 0 && !isInput)) {
            continue;
        }
        updateDeviceSamplerates(device, isInput);
        devices.emplace_back(device);
    }
    return devices;
}

} // namespace

PortAudioClient::PortAudioClient() {
    spdlog::info("Initializing PortAudio...");
    freopen("/dev/null", "w", stderr); // Hides noisy logs from ALSA lib
    PaError err = Pa_Initialize();
    freopen("/dev/tty", "w", stderr);
    checkError(err);
}

PortAudioClient::~PortAudioClient() {
    spdlog::info("Deinitializing PortAudio...");
    if (isStreamOn_) {
        stopStream();
    }
    PaError err = Pa_Terminate();
    checkError(err);
}

std::vector<Device> PortAudioClient::getInputDevices() {
    return getAvailableDevices(/*is_input=*/true);
}

std::vector<Device> PortAudioClient::getOutputDevices() {
    return getAvailableDevices(/*is_input=*/false);
}

void PortAudioClient::startStream(StreamParams input, StreamParams output) {
    isStreamOn_ = true;
}

void PortAudioClient::stopStream() { isTreamOn_ = false; }

void PortAudioClient::setCallback(callback_t callback) {}

} // namespace audio
