#ifndef AUDIO_AUDIO_CLIENT_PORTAUDIO_H
#define AUDIO_AUDIO_CLIENT_PORTAUDIO_H

#include "audio_client.h"
#include <portaudio.h>

namespace audio {

class PortAudioClient : public AudioClient {
  public:
    PortAudioClient();
    ~PortAudioClient() override;
    std::vector<Device> getInputDevices() override;
    std::vector<Device> getOutputDevices() override;
    void startStream(StreamParams input, StreamParams output) override;
    void stopStream() override;
    void setCallback(callback_t callback) override;

  private:
    bool isStreamOn_ = false;
};

} // namespace audio

#endif // AUDIO_AUDIO_CLIENT_PORTAUDIO_H
