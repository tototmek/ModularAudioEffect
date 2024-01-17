#include "main.h"

#include <portaudio.h>
#include <spdlog/spdlog.h>

namespace audio {

typedef struct {
    float left_phase;
    float right_phase;
} paTestData;
static paTestData data;
/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */
static int patestCallback(const void* inputBuffer, void* outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags, void* userData) {
    /* Cast data passed through stream to our structure. */
    paTestData* data = (paTestData*)userData;
    float* out = (float*)outputBuffer;
    unsigned int i;
    (void)inputBuffer; /* Prevent unused variable warning. */

    for (i = 0; i < framesPerBuffer; i++) {
        out[2 * i] = data->left_phase; /* left */
        // out[2 * i] = 0.0f;
        out[2 * i + 1] = data->right_phase; /* right */
        // out[2 * i + 1] = 0.0f;
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

    PaStream* stream;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(
        &stream, 0,                 /* no input channels */
        2,                          /* stereo output */
        paFloat32,                  /* 32 bit floating point output */
        /*sample rate*/ 44100, 256, /* frames per buffer, i.e. the number
                                  of sample frames that PortAudio will
                                  request from the callback. Many apps
                                  may want to use
                                  paFramesPerBufferUnspecified, which
                                  tells PortAudio to pick the best,
                                  possibly changing, buffer size.*/
        patestCallback,             /* this is your callback function */
        &data);                     /*This is a pointer that will be passed to
                                              your callback*/
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

} // namespace audio