#ifndef AUDIO_AUDIO_CLIENT_H
#define AUDIO_AUDIO_CLIENT_H

#include <functional>
#include <string>
#include <vector>

namespace audio {

static const std::vector<double> kStandardSamplerates{
    44100.0, 48000.0, 88200.0, 96000.0, 176400.0, 192000.0};

static const double kSampleRate = 44100.0;

struct Device {
    int index = -1;
    std::string name = "No device";
    uint inputChannels = 0;
    uint outputChannels = 0;
    uint inputLatency = 0;
    uint outputLatency = 0;
    std::vector<double> sampleRates = {};
};

using callback_t =
    std::function<void(const std::vector<float>&, std::vector<float>&)>;

class AudioClient {
  public:
    virtual ~AudioClient(){};
    virtual std::vector<Device> getAvailableInputDevices() = 0;
    virtual std::vector<Device> getAvailableOutputDevices() = 0;
    virtual void startStream() = 0;
    virtual void stopStream() = 0;
    virtual void setCallback(callback_t callback) = 0;
    virtual void setInputDevice(Device device) { inputDevice_ = device; }
    virtual void setOutputDevice(Device device) { outputDevice_ = device; }
    virtual double getSampleRate() const { return kSampleRate; }
    virtual Device getInputDevice() const { return inputDevice_; }
    virtual Device getOutputDevice() const { return outputDevice_; }

  private:
    Device inputDevice_;
    Device outputDevice_;
};

} // namespace audio

#endif // AUDIO_AUDIO_CLIENT_H
