#include "Part1.h"


MainContentComponent::MainContentComponent() {
    titleLabel.setText("Part1", juce::NotificationType::dontSendNotification);
    titleLabel.setSize(160, 40);
    titleLabel.setFont(juce::Font(36, juce::Font::FontStyleFlags::bold));
    titleLabel.setJustificationType(juce::Justification(juce::Justification::Flags::centred));
    titleLabel.setCentrePosition(300, 40);
    addAndMakeVisible(titleLabel);

    recordButton.setButtonText("Record");
    recordButton.setSize(80, 40);
    recordButton.setCentrePosition(150, 140);
    recordButton.onClick = [this] {
        if (this->status == 0) {
            this->status = 1;
            this->startTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        }
    };
    addAndMakeVisible(recordButton);

    playbackButton.setButtonText("Play");
    playbackButton.setSize(80, 40);
    playbackButton.setCentrePosition(300, 140);
    playbackButton.onClick = [this] {
        if (this->status == 2) {
            this->status = 3;
            this->startTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
        }
    };
    addAndMakeVisible(playbackButton);

    playTune.setButtonText("CK2");
    playTune.setSize(80, 40);
    playTune.setCentrePosition(450, 140);
    playTune.onClick = [this] { this->playBackGround = !this->playBackGround; };
    addAndMakeVisible(playTune);


    setSize(600, 300);
    setAudioChannels(1, 1);
}

void MainContentComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    this->_sampleRate = sampleRate;
    this->delayBuffer = new juce::AudioSampleBuffer;
    this->delayBuffer->setSize(1, (int) this->getSampleRate() * 10);// 10 seconds in total
}

void MainContentComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
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
            float dPhasePerSample1 = (float)(PI * 2.0 * ((double) freq1 / (double) sampleRate));
            float dPhasePerSample2 = (float)(PI * 2.0 * ((double) freq2 / (double) sampleRate));
            float initPhase = 0;

            for (int i = 0; i < buffer->getNumSamples(); i++) {
                outBuffer[i] += amp * sin(dPhasePerSample1 * (float) i + initPhase) + amp * sin(dPhasePerSample2 * (float) i + initPhase);
            }
        }
    }

    switch (status) {
        case 0:
            titleLabel.setText("Part1", juce::NotificationType::dontSendNotification);
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

void MainContentComponent::fillBuffer(int channel, int bufferSize, int delayBufferSize, const float *channelData) {
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

void MainContentComponent::readFromBuffer(int channel, int bufferSize, int delayBufferSize, juce::AudioSampleBuffer *buffer) {
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

void MainContentComponent::releaseResources() {
    this->delayBuffer->setSize(0, 0);
    delete &this->delayBuffer;
}
