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
#define FUZZ(x) CubicAmplifier(CubicAmplifier(CubicAmplifier(x)))
static int patestCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData) {
    /* Cast data passed through stream to our structure. */
    const float* in = (const float*)inputBuffer;
    float* out = (float*)outputBuffer;
    PortAudioClient* client = reinterpret_cast<PortAudioClient*>(userData);
    uint inputChannels = client->nInputChannels();
    uint outputChannels = client->nOutputChannels();
    if (outputChannels == 0) {
        return 0;
    }
    if (inputChannels == 0) {
        std::fill(out, out + framesPerBuffer, 0.0f);
        return 0;
    }
    unsigned int i;
    for (i = 0; i < framesPerBuffer; i++) {
        in++;
        float sample = FUZZ(*in++);
        out[2 * i] = sample;
        out[2 * i + 1] = sample;
    }
    return 0;
}

Device getDeviceAt(uint index) {
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
            params.channelCount = device.inputChannels;
            result = Pa_IsFormatSupported(&params, nullptr, samplerate);
        } else {
            params.channelCount = device.outputChannels;
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

PaStreamParameters* getDeviceStreamParameters(Device device, bool isInput) {
    if (device.index == -1) {
        return nullptr;
    }
    PaStreamParameters* params = new PaStreamParameters;
    params->device = device.index;
    if (isInput) {
        params->channelCount = device.inputChannels;
        params->suggestedLatency =
            Pa_GetDeviceInfo(device.index)->defaultLowInputLatency;
    } else {
        params->channelCount = device.outputChannels;
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

void PortAudioClient::streamCallback(const std::vector<float>& inputSamples,
                                     std::vector<float>& outputSamples) {}

void PortAudioClient::startStream() {
    if (isStreamRunning_) {
        spdlog::warn("Cannot start stream: Stream already running");
        return;
    }
    if (getInputDevice().index == -1 && getOutputDevice().index == -1) {
        spdlog::warn("Cannot start stream: No device selected");
        return;
    }
    auto inputParams = getDeviceStreamParameters(getInputDevice(), true);
    auto outputParams = getDeviceStreamParameters(getOutputDevice(), false);
    spdlog::info(
        "Starting stream: Sample rate: {} Hz, Input: {}. {}, Output: {}. {}",
        getSampleRate(), getInputDevice().index, getInputDevice().name,
        getOutputDevice().index, getOutputDevice().name);
    PaError e = Pa_OpenStream(&stream_, inputParams, outputParams,
                              getSampleRate(), paFramesPerBufferUnspecified,
                              paNoFlag, patestCallback, this);
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

void PortAudioClient::setCallback(callback_t callback) {}

} // namespace audio
