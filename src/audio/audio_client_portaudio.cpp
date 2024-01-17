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

static int period = 0;

static int portAudioCallback(const void* inputBufferVoidPtr,
                             void* outputBufferVoidPtr,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData) {

    // PortAudioClient* instance = reinterpret_cast<PortAudioClient*>(userData);
    // const float* inputBuffer =
    // reinterpret_cast<const float*>(inputBufferVoidPtr);
    float* outputBuffer = (float*)outputBufferVoidPtr;
    // uint nInputs = instance->getInputDevice().inputChannels;
    // uint nOutputs = instance->getOutputDevice().outputChannels;
    // spdlog::info("{}: {} inputs", instance->getInputDevice().name, nInputs);
    // spdlog::info("{}: {} outputs", instance->getOutputDevice().name,
    // nOutputs); std::vector<float> inputVector(nInputs, 0.0f);
    // std::vector<float> outputVector(nOutputs, 0.0f);
    // uint inputPosition = 0;
    // uint outputPosition = 0;
    for (uint i = 0; i < framesPerBuffer; ++i) {
        period++;
        // float sample = inputBuffer[i * 2 + 1];
        outputBuffer[i * 2 + 1] = sinf(period * 0.05f);
        // outputBuffer[i * 2] = period * 0.005f - 0.5f;
        outputBuffer[i * 2] = 0.0f;
        // spdlog::info("Sample: {}", sample);
        // if (nInputs > 0) {
        // inputVector =
        // std::vector<float>(inputBuffer + inputPosition,
        //    inputBuffer + inputPosition + nInputs - 1);
        // inputPosition += nInputs;
        // }
        // instance->streamCallback(inputVector, outputVector);
        // if (nOutputs > 0) {
        // std::copy(outputVector.begin(), outputVector.end(),
        //   outputBuffer + outputPosition);
        // }
    }
    return paContinue;
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

std::vector<Device> PortAudioClient::getAvailableInputDevices() {
    return getAvailableDevices(/*is_input=*/true);
}

std::vector<Device> PortAudioClient::getAvailableOutputDevices() {
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
}

void PortAudioClient::streamCallback(const std::vector<float>& inputSamples,
                                     std::vector<float>& outputSamples) {}

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
        paFramesPerBufferUnspecified, paNoFlag, portAudioCallback, this);
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
    PaError e = Pa_AbortStream(stream_);
    checkError(e);
    e = Pa_CloseStream(stream_);
    checkError(e);
}

void PortAudioClient::setCallback(callback_t callback) {}

} // namespace audio
