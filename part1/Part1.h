#include <JuceHeader.h>
#include <chrono>
#pragma once

#define PI 3.14159

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;


class MainContentComponent : public juce::AudioAppComponent {
public:
    MainContentComponent();

    ~MainContentComponent() override { shutdownAudio(); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        this->_sampleRate = sampleRate;
        this->delayBuffer = new juce::AudioSampleBuffer;
        this->delayBuffer->setSize(1, (int) this->getSampleRate() * 10);// 10 seconds in total
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        auto *device = deviceManager.getCurrentAudioDevice();
        auto activeInputChannels = device->getActiveInputChannels();
        auto activeOutputChannels = device->getActiveOutputChannels();
        auto maxInputChannels = activeInputChannels.getHighestBit() + 1;
        auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;
        auto buffer = bufferToFill.buffer;
        auto delayBufferSize = delayBuffer->getNumSamples();
        auto bufferSize = buffer->getNumSamples();

        constexpr const long long recordingLength = 10 * 1000;
        if (status == 1 && duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - startTime > recordingLength) {
            status = 2;
            startTime = 0;
        } else if (status == 3 && duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - startTime > recordingLength) {
            status = 0;
            startTime = 0;
        }

        for (auto channel = 0; channel < maxOutputChannels; ++channel) {
            if ((!activeInputChannels[channel] || !activeOutputChannels[channel]) || maxInputChannels == 0) {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            } else {
                auto channelData = buffer->getReadPointer(channel);
                if (status == 1) {
                    fillBuffer(channel, bufferSize, delayBufferSize, channelData);
                    buffer->clear();
                } else if (status == 3) {
                    readFromBuffer(channel, bufferSize, delayBufferSize, buffer);
                } else {
                    buffer->clear();
                }
            }

            if (playBackGround) {
                auto *outBuffer = buffer->getWritePointer(channel, bufferToFill.startSample);
                int freq1 = 1000;
                int freq2 = 10000;
                float amp = 1;
                double sampleRate = this->getSampleRate();
                float dPhasePerSample1 = PI * 2.0f * ((float) freq1 / (float) sampleRate);
                float dPhasePerSample2 = PI * 2.0f * ((float) freq2 / (float) sampleRate);
                float initPhase = 0;

                for (int i = 0; i < buffer->getNumSamples(); i++) {
                    outBuffer[i] += amp * sin(dPhasePerSample1 * (float) i + initPhase) + amp * sin(dPhasePerSample2 * (float) i + initPhase);
                }
            }
        }

        switch (status) {
            case 0:
                titleLabel.setText("Part1&2", juce::NotificationType::dontSendNotification);
                break;
            case 1:
                titleLabel.setText("Recording", juce::NotificationType::dontSendNotification);
                break;
            case 2:
                titleLabel.setText("Recorded", juce::NotificationType::dontSendNotification);
                break;
            case 3:
                titleLabel.setText("Playing", juce::NotificationType::dontSendNotification);
                break;
        }
    }

    void fillBuffer(int channel, int bufferSize, int delayBufferSize, const float *channelData) {
        float gain = 1.0;
        if (delayBufferSize > bufferSize + writePosition) {
            delayBuffer->copyFromWithRamp(channel, writePosition, channelData, bufferSize, gain, gain);
        } else {
            auto numSamplesToEnd = delayBufferSize - writePosition;
            delayBuffer->copyFromWithRamp(channel, writePosition, channelData, numSamplesToEnd, gain, gain);
            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            delayBuffer->copyFromWithRamp(channel, 0, channelData + numSamplesToEnd, numSamplesAtStart, gain, gain);
        }
        writePosition += bufferSize;
        writePosition %= delayBufferSize;
    }

    void readFromBuffer(int channel, int bufferSize, int delayBufferSize, juce::AudioSampleBuffer *buffer) {
        float gain = 1.0;
        buffer->clear();
        if (readPosition + bufferSize < delayBufferSize) {
            buffer->addFromWithRamp(channel, 0, delayBuffer->getReadPointer(channel, (int) readPosition), bufferSize, gain, gain);
        } else {
            auto numSamplesToEnd = delayBufferSize - readPosition;
            buffer->addFromWithRamp(channel, 0, delayBuffer->getReadPointer(channel, (int) readPosition), (int) numSamplesToEnd, gain, gain);
            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            buffer->addFromWithRamp(channel, (int) numSamplesToEnd, delayBuffer->getReadPointer(channel, 0), (int) numSamplesAtStart, gain, gain);
        }
        readPosition += bufferSize;
        readPosition %= delayBufferSize;
    }

    void releaseResources() override {
        this->delayBuffer->setSize(0, 0);
        delete &this->delayBuffer;
    }

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
