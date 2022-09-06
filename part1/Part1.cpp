#include "Part1.h"


MainContentComponent::MainContentComponent() {
    titleLabel.setText("Part1&2", juce::NotificationType::dontSendNotification);
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

    playTune.setButtonText("Part2");
    playTune.setSize(80, 40);
    playTune.setCentrePosition(450, 140);
    playTune.onClick = [this] { this->playBackGround = !this->playBackGround; };
    addAndMakeVisible(playTune);


    setSize(600, 300);
    setAudioChannels(1, 1);
}
