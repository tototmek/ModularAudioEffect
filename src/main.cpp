#include "audio/audio_client_portaudio.h"
#include <memory>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#define MAIN2

#ifdef MAIN1
/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */

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
    unsigned int i;

    for (i = 0; i < framesPerBuffer; i++) {
        in++;
        float sample = FUZZ(*in++);
        out[2 * i] = sample; /* left */
        out[2 * i + 1] = sample;
    }
    return 0;
}

void handleError(const PaError& error) {
    spdlog::error("PortAudio error: {}", Pa_GetErrorText(error));
    exit(1);
}

int main() {
    spdlog::info("Initializing PortAudio...");
    freopen("/dev/null", "w", stderr);
    PaError err = Pa_Initialize();
    freopen("/dev/tty", "w", stderr);
    if (err != paNoError)
        handleError(err);

    uint output_index = 7;
    PaStreamParameters outputParams;
    outputParams.device = output_index;
    outputParams.channelCount = 2;
    outputParams.hostApiSpecificStreamInfo = nullptr;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(output_index)->defaultLowOutputLatency;

    uint input_index = 10;
    PaStreamParameters inputParams;
    inputParams.device = input_index;
    inputParams.channelCount = 2;
    inputParams.hostApiSpecificStreamInfo = nullptr;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(input_index)->defaultLowInputLatency;

    PaStream* stream;
    err = Pa_OpenStream(&stream, &inputParams, &outputParams, 44100.0,
                        paFramesPerBufferUnspecified, paNoFlag, patestCallback,
                        nullptr);
    if (err != paNoError)
        handleError(err);

    err = Pa_StartStream(stream);
    if (err != paNoError)
        handleError(err);

    Pa_Sleep(5000);
    err = Pa_StopStream(stream);
    if (err != paNoError)
        handleError(err);

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        handleError(err);

    err = Pa_Terminate();
    if (err != paNoError)
        handleError(err);

    return 0;
}
#endif
#ifdef MAIN2

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
    client.setOutputDevice(outputDevices[10]);
    client.startStream();
    Pa_Sleep(3000);
    client.stopStream();
    client.setInputDevice(inputDevices[3]);
    client.setOutputDevice(outputDevices[10]);
    client.startStream();
    Pa_Sleep(3000);
    client.setOutputDevice(outputDevices[8]);
    Pa_Sleep(1000);
    client.stopStream();
    client.startStream();
    Pa_Sleep(3000);
    client.setOutputDevice(outputDevices[0]);
    Pa_Sleep(1000);
    client.stopStream();
    client.startStream();
    Pa_Sleep(1000);
}

#endif