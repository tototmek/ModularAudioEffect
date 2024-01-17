#include "audio/audio_client_portaudio.h"
#include <memory>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
typedef struct {
    float left_phase;
    float right_phase;
} paTestData;
static paTestData data;
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
    paTestData* data = (paTestData*)userData;
    const float* in = (const float*)inputBuffer;
    float* out = (float*)outputBuffer;
    unsigned int i;

    for (i = 0; i < framesPerBuffer; i++) {
        in++;
        // spdlog::info("{}", *in);
        float sample = FUZZ(*in++);
        out[2 * i] = sample; /* left */
        // out[2 * i] = 0.0f;
        // out[2 * i + 1] = data->right_phase; /* right */
        out[2 * i + 1] = sample;
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if (data->left_phase >= 1.0f)
            data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if (data->right_phase >= 1.0f)
            data->right_phase -= 2.0f;
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

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        spdlog::error("Error: Pa_GetDeviceCount failed.");
        Pa_Terminate();
        return 1;
    }

    spdlog::info("Number of audio devices: {}", numDevices);

    for (int i = 0; i < numDevices; ++i) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        spdlog::info("Device {}: {}", i, deviceInfo->name);
    }

    uint output_index = 10;
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
                        &data);
    if (err != paNoError)
        handleError(err);

    err = Pa_StartStream(stream);
    if (err != paNoError)
        handleError(err);

    Pa_Sleep(50000);
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
