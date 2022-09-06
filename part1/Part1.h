#include <JuceHeader.h>
#include <chrono>
#include <cmath>
#pragma once

#define PI acos(-1)

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;


class MainContentComponent : public juce::AudioAppComponent {
public:
    MainContentComponent();

    ~MainContentComponent() override { shutdownAudio(); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    void fillBuffer(int channel, int bufferSize, int delayBufferSize, const float *channelData);

    void readFromBuffer(int channel, int bufferSize, int delayBufferSize, juce::AudioSampleBuffer *buffer);

    void releaseResources() override;

    double getSampleRate() const { return _sampleRate; }

private:
    juce::Label titleLabel;
    juce::TextButton recordButton;
    juce::TextButton playbackButton;
    juce::TextButton playTune;

    int status{0};// 0 for not started, 1 for recording, 2 for finished recording, 3 for playing.
    long long startTime{0};
    double _sampleRate{0};
    juce::AudioSampleBuffer *delayBuffer{nullptr};
    int writePosition{0};
    int readPosition{0};
    bool playBackGround{false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
