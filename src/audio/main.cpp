#include "main.h"

#include <spdlog/spdlog.h>

namespace audio {

// void error(const PaError& error) {
//     spdlog::error("PortAudio error: {}", Pa_GetErrorText(error));
//     exit(1);
// }
int main() {
    spdlog::info("Initializing PortAudio...");
    // PaError e = Pa_Initialize();
    // if (e != paNoError)
    //     error(e);

    // e = Pa_Terminate();
    // if (e != paNoError)
    //     error(e);

    return 0;
}

} // namespace audio