#ifndef AUDIO_AUDIO_CLIENT_PORTAUDIO_H
#define AUDIO_AUDIO_CLIENT_PORTAUDIO_H

#include "audio_client.h"
#include <portaudio.h>

namespace audio {

class PortAudioClient : public AudioClient {
  public:
    PortAudioClient();
    ~PortAudioClient() override;
    std::vector<Device> getAvailableInputDevices() override;
    std::vector<Device> getAvailableOutputDevices() override;
    void startStream() override;
    void stopStream() override;
    void setCallback(callback_t callback) override;
    void setInputDevice(Device device) override;
    void setOutputDevice(Device device) override;
    void streamCallback(const std::vector<float>& inputSamples,
                        std::vector<float>& outputSamples);
    PaStream* getStream() { return stream_; }

  private:
    PaStream* stream_ = nullptr;
    PaStreamParameters* inputParamsSelected_ = nullptr;
    PaStreamParameters* outputParamsSelected_ = nullptr;
    PaStreamParameters inputParams_;
    PaStreamParameters outputParams_;
    bool isStreamRunning_ = false;
};

} // namespace audio

#endif // AUDIO_AUDIO_CLIENT_PORTAUDIO_H
