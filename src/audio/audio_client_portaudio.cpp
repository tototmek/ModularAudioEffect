#include "audio_client_portaudio.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace audio {

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
    devices.emplace_back(Device{});
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
    stopStream();
    spdlog::info("Deinitializing PortAudio...");
    PaError err = Pa_Terminate();
    checkError(err);
}

std::vector<Device> PortAudioClient::getInputDevices() {
    return getAvailableDevices(/*is_input=*/true);
}

std::vector<Device> PortAudioClient::getOutputDevices() {
    return getAvailableDevices(/*is_input=*/false);
}

void PortAudioClient::setInputDevice(Device device) {
    AudioClient::setInputDevice(device);
    if (device.index == -1) { // "No device" option selected
        inputParamsSelected_ = nullptr;
    } else {
        inputParams_.device = device.index;
        inputParams_.channelCount = device.inputChannels;
        inputParams_.suggestedLatency = device.inputLatency;
        inputParams_.sampleFormat = paFloat32;
        inputParams_.hostApiSpecificStreamInfo = nullptr;
        inputParamsSelected_ = &inputParams_;
    }
    stopStream();
    startStream();
}

void PortAudioClient::setOutputDevice(Device device) {
    AudioClient::setOutputDevice(device);
    if (device.index == -1) { // "No device" option selected
        outputParamsSelected_ = nullptr;
    } else {
        outputParams_.device = device.index;
        outputParams_.channelCount = device.outputChannels;
        outputParams_.suggestedLatency = device.outputLatency;
        outputParams_.sampleFormat = paFloat32;
        outputParams_.hostApiSpecificStreamInfo = nullptr;
        outputParamsSelected_ = &outputParams_;
    }
    stopStream();
    startStream();
}

static int patestCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData) {
    return 0;
}

void PortAudioClient::startStream() {
    if (isStreamRunning_ || // Stream is already started or no device selected
        (inputParamsSelected_ == nullptr && outputParamsSelected_ == nullptr)) {
        return;
    }
    spdlog::info("Starting stream: Sample rate: {} Hz, Input: {}, Output:{}",
                 getSampleRate(), getInputDevice().name,
                 getOutputDevice().name);
    PaError e = Pa_OpenStream(
        &stream_, inputParamsSelected_, outputParamsSelected_, getSampleRate(),
        paFramesPerBufferUnspecified, 0, patestCallback, nullptr);
    checkError(e);
    e = Pa_StartStream(stream_);
    checkError(e);
    isStreamRunning_ = true;
}

void PortAudioClient::stopStream() {
    if (!isStreamRunning_) {
        return;
    }
    isStreamRunning_ = false;
    PaError e = Pa_StopStream(stream_);
    checkError(e);
    e = Pa_CloseStream(stream_);
    checkError(e);
}

void PortAudioClient::setCallback(callback_t callback) {}

} // namespace audio
