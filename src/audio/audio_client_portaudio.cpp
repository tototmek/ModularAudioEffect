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

static int portAudioCallback(const void* inputBuffer, void* outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData) {
    static std::vector<float> inputVector, outputVector;
    (void)timeInfo;
    const float* in = (const float*)inputBuffer;
    float* out = (float*)outputBuffer;
    PortAudioClient* client = reinterpret_cast<PortAudioClient*>(userData);
    uint nInputs = client->getInputDevice().inputChannels;
    uint nOutputs = client->getOutputDevice().outputChannels;
    inputVector.resize(nInputs);
    outputVector.resize(nOutputs);
    uint inIndex = 0, outIndex = 0, i = 0, j = 0, k = 0;
    for (i = 0; i < framesPerBuffer; ++i) {
        for (j = 0; j < nInputs; ++j) {
            inputVector[j] = in[inIndex++];
        }
        client->callCallback(inputVector, outputVector);
        for (k = 0; k < nOutputs; ++k) {
            out[outIndex++] = outputVector[k];
        }
    }
    if (statusFlags & paOutputUnderflow) {
        spdlog::error("Output underflow detected. (xrun)");
    }
    if (statusFlags & paInputOverflow) {
        spdlog::error("Input overflow detected. (xrun)");
    }
    return paContinue;
}

Device getDeviceAt(int index) {
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(index);
    Device device;
    device.index = index;
    device.name = std::string(deviceInfo->name);
    device.inputChannels =
        std::min(uint(deviceInfo->maxInputChannels), kMaxChannels);
    device.outputChannels =
        std::min(uint(deviceInfo->maxOutputChannels), kMaxChannels);
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
        params.suggestedLatency = 0;
        if (isInput) {
            params.channelCount = int(device.inputChannels);
            result = Pa_IsFormatSupported(&params, nullptr, samplerate);
        } else {
            params.channelCount = int(device.outputChannels);
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
        Device device = getDeviceAt(int(i));
        if ((device.inputChannels == 0 && isInput) ||
            (device.outputChannels == 0 && !isInput)) {
            continue;
        }
        updateDeviceSamplerates(device, isInput);
        devices.emplace_back(device);
    }
    return devices;
}

PaStreamParameters* getDeviceStreamParameters(Device device, bool isInput) {
    if (device.index == -1) {
        return nullptr;
    }
    PaStreamParameters* params = new PaStreamParameters;
    params->device = device.index;
    if (isInput) {
        params->channelCount = int(device.inputChannels);
        params->suggestedLatency =
            Pa_GetDeviceInfo(device.index)->defaultLowInputLatency;
    } else {
        params->channelCount = int(device.outputChannels);
        params->suggestedLatency =
            Pa_GetDeviceInfo(device.index)->defaultLowOutputLatency;
    }
    params->sampleFormat = paFloat32;
    params->hostApiSpecificStreamInfo = nullptr;
    return params;
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

void PortAudioClient::callCallback(const std::vector<float>& input,
                                   std::vector<float>& output) {
    if (callback_ == nullptr) {
        spdlog::warn("PortAudio callback is an empty function "
                     "(PortAudioClient::callback_ = nullptr)");
        return;
    }

    callback_(input, output);
}

void PortAudioClient::startStream() {
    if (isStreamRunning_) {
        spdlog::warn("Cannot start stream: Stream already running");
        return;
    }
    if (getInputDevice().index == -1 && getOutputDevice().index == -1) {
        spdlog::warn("Cannot start stream: No device selected");
        return;
    }
    auto* inputParams = getDeviceStreamParameters(getInputDevice(), true);
    auto* outputParams = getDeviceStreamParameters(getOutputDevice(), false);
    spdlog::info("Starting stream: Sample rate: {} Hz, Input: {}. {}, "
                 "Output: {}. {}",
                 getSampleRate(), getInputDevice().index, getInputDevice().name,
                 getOutputDevice().index, getOutputDevice().name);
    PaError e = Pa_OpenStream(&stream_, inputParams, outputParams,
                              getSampleRate(), paFramesPerBufferUnspecified,
                              paNoFlag, portAudioCallback, this);
    if (inputParams != nullptr) {
        nInputChannels_ = inputParams->channelCount;
        delete inputParams;
    } else {
        nInputChannels_ = 0;
    }
    if (outputParams != nullptr) {
        nOutputChannels_ = outputParams->channelCount;
        delete outputParams;
    } else {
        nOutputChannels_ = 0;
    }
    checkError(e);
    e = Pa_StartStream(stream_);
    checkError(e);
    isStreamRunning_ = true;
}

void PortAudioClient::stopStream() {
    if (!isStreamRunning_) {
        spdlog::warn("Cannot stop stream: Stream not running");
        return;
    }
    isStreamRunning_ = false;
    PaError e = Pa_AbortStream(stream_);
    checkError(e);
    e = Pa_CloseStream(stream_);
    checkError(e);
}

void PortAudioClient::setCallback(callback_t callback) { callback_ = callback; }

} // namespace audio
