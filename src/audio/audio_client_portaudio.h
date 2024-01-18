#ifndef AUDIO_AUDIO_CLIENT_PORTAUDIO_H
#define AUDIO_AUDIO_CLIENT_PORTAUDIO_H

#include "audio_client.h"
#include <portaudio.h>

namespace audio {

constexpr uint kMaxChannels = 2;

class PortAudioClient : public AudioClient {
  public:
    PortAudioClient();
    ~PortAudioClient() override;
    std::vector<Device> getAvailableInputDevices() override;
    std::vector<Device> getAvailableOutputDevices() override;
    void startStream() override;
    void stopStream() override;
    void setCallback(callback_t callback) override;
    uint nInputChannels() { return nInputChannels_; }
    uint nOutputChannels() { return nOutputChannels_; }
    void callCallback(const std::vector<float>& input,
                      std::vector<float>& output);

  private:
    callback_t callback_;
    PaStream* stream_ = nullptr;
    bool isStreamRunning_ = false;
    uint nInputChannels_ = 0;
    uint nOutputChannels_ = 0;
};

} // namespace audio

#endif // AUDIO_AUDIO_CLIENT_PORTAUDIO_H
