#ifndef AUDIO_AUDIO_CLIENT_H
#define AUDIO_AUDIO_CLIENT_H

#include <functional>
#include <string>
#include <vector>

namespace audio {

struct Device {
    uint index = 0;
    std::string name;
    uint inputChannels;
    uint outputChannels;
    uint inputLatency;
    uint outputLatency;
    std::vector<double> sampleRates;
};

struct StreamParams {
    Device device;
    double samplerate;
    double latency;
};

using callback_t =
    std::function<void(const std::vector<float>&, std::vector<float>&)>;

class AudioClient {
  public:
    virtual ~AudioClient(){};
    virtual std::vector<Device> getInputDevices() = 0;
    virtual std::vector<Device> getOutputDevices() = 0;
    virtual void startStream(StreamParams input, StreamParams output) = 0;
    virtual void stopStream() = 0;
    virtual void setCallback(callback_t callback) = 0;
};

} // namespace audio

#endif // AUDIO_AUDIO_CLIENT_H
