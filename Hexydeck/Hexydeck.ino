#include <MultiMap.h>
#include <Button2.h>
#include "FastLED.h"
#include <Wire.h>
#include <Adafruit_MPR121.h>
#include "ScaleManager.h"
#include <NewPing.h>
#include "drumSamples.h"
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "Mapf.h"
#include "formant.h"
#include "synthesis.h"

// GUItool: end automatically generated code
const int PLAY_BUTTON = 35;
const int TEMPO_UP_BUTTON = 37;
const int TEMPO_DOWN_BUTTON = 36;

const int HAND[] = { 38, 39, 40 };

const int PATTERN_CONTROL_PIN[3] = { 41, 14, 16 };
const int DRUM_CONTROL_PIN[3] = { 25, 26, 27 };

Button2 playButton, tempoUpButton, tempoDownButton, handOne, handTwo, handThree;

bool handOn[3];

//MIDI CHANNELS
const char MELODY_CHANNEL = 1;
const char BASS_CHANNEL = 2;
const char DRUM_CHANNEL = 3;
const char CHORD_CHANNEL = 4;

ScaleManager sm(true);

#define NUM_LEDS 49
const int LED_DATA_PIN = 24;
CRGBArray<NUM_LEDS> leds;

CRGBSet sonarLED(leds(0, 3));

CRGBSet kickSequenceLED(leds(4, 11));
CRGBSet snareSequenceLED(leds(12, 19));
CRGBSet hatSequenceLED(leds(20, 27));

CRGBSet smallHandLED(leds(28, 30));
CRGBSet bigHandLED(leds(46, 48));

CRGBSet noteRowOneLEDS(leds(31, 34));
CRGBSet noteRowTwoLEDS(leds(35, 37));
CRGBSet noteRowThreeLEDS(leds(38, 39));

CRGBSet drumFaderLED(leds(40, 42));
CRGBSet arcadeButtonLED(leds(43, 45));

CRGBSet lightSegment[] = { leds(0, 3), leds(4, 27), leds(28, 30), leds(31, 39), leds(40, 45), leds(46, 48) };


class Sequencer {
public:
  Sequencer() {
    currentKickSequence = kickSequence[1];
    currentSnareSequence = snareSequence[2];
    currentHatSequence = hatSequence[0];
    delay(10);
  };

  bool isPlaying = false;
  int tempo = 100;

  const static int NUM_OF_SEQUENCES = 8;
  uint8_t kickSequence[NUM_OF_SEQUENCES] = { 0b00000001, 0b00010001, 0b0011001, 0b00101001, 0b10101001, 0b01001001, 0b00111001, 0b10100010 };
  uint8_t snareSequence[NUM_OF_SEQUENCES] = { 0b00010000, 0b00011000, 0b01000100, 0b10101000, 0b11110000, 0b00101000, 0b10101010, 0b11111111 };
  uint8_t hatSequence[NUM_OF_SEQUENCES] = { 0b01010101, 0b11110000, 0b10110000, 0b11001100, 0b11100010, 0b00001111, 0b00100100, 0b11111111 };

  uint8_t currentKickSequence;
  uint8_t currentSnareSequence;
  uint8_t currentHatSequence;

  uint32_t timeForNextBeat = 0;
  elapsedMillis beatClock;

  void togglePlay() {
    isPlaying = !isPlaying;
    beatIndex = 0;
  };

  void update() {

    currentKickSequence = kickSequence[map(analogRead(PATTERN_CONTROL_PIN[2]), 0, 1023, 0, NUM_OF_SEQUENCES)];
    currentSnareSequence = snareSequence[map(analogRead(PATTERN_CONTROL_PIN[1]), 0, 1023, 0, NUM_OF_SEQUENCES)];
    currentHatSequence = hatSequence[map(analogRead(PATTERN_CONTROL_PIN[0]), 0, 1023, 0, NUM_OF_SEQUENCES)];

    if (beatClock > timeForNextBeat && isPlaying) {
      beatIndex++;
      if (beatIndex >= 8) beatIndex = 0;

      if (bitRead(currentKickSequence, beatIndex)) {
        kickSample.play(AudioSampleKickaccent);
        usbMIDI.sendNoteOn(36, 100, DRUM_CHANNEL);
        usbMIDI.sendNoteOff(36, 0, DRUM_CHANNEL);
      }

      if (bitRead(currentSnareSequence, beatIndex)) {
        snareSample.play(AudioSampleSnare);
        usbMIDI.sendNoteOn(38, 100, DRUM_CHANNEL);
        usbMIDI.sendNoteOff(38, 0, DRUM_CHANNEL);
      }

      if (bitRead(currentHatSequence, beatIndex)) {
        hatSample.play(AudioSampleHihataccent);
        if (handOn[0] || handOn[1] || handOn[2]) handSawEnvelope.noteOn();
        usbMIDI.sendNoteOn(42, 100, DRUM_CHANNEL);
        usbMIDI.sendNoteOff(42, 0, DRUM_CHANNEL);
      }


      beatClock = 0;
      timeForNextBeat = 60000 / tempo / 4;
      // return true;
    }
  }

  void increaseTempo() {
    tempo++;
    if (tempo > maxTempo) tempo = maxTempo;
    timeForNextBeat = 60000 / tempo / 2;
  };

  void decreaseTempo() {
    tempo--;
    if (tempo < minTempo) tempo = minTempo;
    timeForNextBeat = 60000 / tempo / 2;
  };

  uint8_t beatIndex = 0;

private:
  int maxTempo = 180;
  int minTempo = 60;
};

Sequencer sequencer;

#define TOUCH 20
#define RELEASE 10

Adafruit_MPR121 cap5A = Adafruit_MPR121();
Adafruit_MPR121 cap5B = Adafruit_MPR121();
Adafruit_MPR121 cap5C = Adafruit_MPR121();
Adafruit_MPR121 cap5D = Adafruit_MPR121();

uint16_t lasttouched5A = 0;
uint16_t currtouched5A = 0;
uint16_t lasttouched5B = 0;
uint16_t currtouched5B = 0;
uint16_t lasttouched5C = 0;
uint16_t currtouched5C = 0;
uint16_t lasttouched5D = 0;
uint16_t currtouched5D = 0;

bool noteIsOn[48];

uint16_t baselineReading[48];
uint16_t filteredReading[48];

void setup() {

  Serial.begin(115200);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  Serial.print("Hexy Beats v1.00");
  Serial.print("----------------");
  // Arcade Buttons
  setupButtons();
  // MPR121 Capacitive Touch sensors
  setupTouch();
  // FastLED Setup and Light Sequence
  setupLED();
  // // Scale Manager initialises
  setupScale();

  setupSonar();

  updateSequencerLEDS();

  audioSetup();

  bigHandLED = CRGB::Black;
  smallHandLED = CRGB::Black;
  smallHandLED = CRGB::Black;
  noteRowOneLEDS = CRGB::Black;
  noteRowTwoLEDS = CRGB::Black;
  noteRowThreeLEDS = CRGB::Black;

  drumFaderLED[2] = CRGB::Red;
  drumFaderLED[0] = CRGB::Orange;
  drumFaderLED[1] = CRGB::BlueViolet;
}

void loop() {
  static elapsedMillis functionTimer;

  if ((sequencer.beatClock + 16) < sequencer.timeForNextBeat || !sequencer.isPlaying) sonarUpdate();  //if pinging the ultrasonic sensors is going to delay next beat, skip the ping.
  // Serial.print("1");
  checkButtons();
  // Serial.print("2");
  checkKnobs();
  // Serial.print("3");
  updateTouch();
  // Serial.print("4");
  sequencer.update();
  // Serial.print("5");
  updateSequencerLEDS();
  // Serial.print("6");
  updateStringLEDs();
  fadeToBlackBy(sonarLED, 4, 80);
  FastLED.show();
}

void setupTouch() {
  delay(500);

  Serial.print("Touch Sensor Setup...");


  if (cap5B.begin(0x5B)) Serial.print("2");
  if (cap5C.begin(0x5C)) Serial.print("3");
  if (cap5D.begin(0x5D)) Serial.print("4");
  if (cap5A.begin(0x5A)) Serial.print("1");
  delay(1000);

  cap5A.setThresholds(TOUCH, RELEASE);
  cap5B.setThresholds(TOUCH, RELEASE);
  cap5C.setThresholds(TOUCH, RELEASE);
  cap5D.setThresholds(TOUCH, RELEASE);

  delay(500);

  Serial.println("...Complete!");
}

void setupButtons() {

  for (int i = 0; i < 3; i++) {
    pinMode(DRUM_CONTROL_PIN[i], INPUT);
    pinMode(PATTERN_CONTROL_PIN[i], INPUT);
  }

  playButton.begin(PLAY_BUTTON);
  tempoDownButton.begin(TEMPO_DOWN_BUTTON);
  tempoUpButton.begin(TEMPO_UP_BUTTON);

  handOne.begin(HAND[0]);
  handTwo.begin(HAND[1]);
  handThree.begin(HAND[2]);

  pinMode(PLAY_BUTTON, INPUT_PULLUP);
  pinMode(TEMPO_DOWN_BUTTON, INPUT_PULLUP);
  pinMode(TEMPO_UP_BUTTON, INPUT_PULLUP);

  pinMode(HAND[0], INPUT);
  pinMode(HAND[1], INPUT);
  pinMode(HAND[2], INPUT);

  playButton.setPressedHandler(pressedHandler);
  tempoUpButton.setPressedHandler(pressedHandler);
  tempoDownButton.setPressedHandler(pressedHandler);

  handOne.setPressedHandler(pressedHandler);
  handOne.setReleasedHandler(releasedHandler);

  handTwo.setPressedHandler(pressedHandler);
  handTwo.setReleasedHandler(releasedHandler);

  handThree.setPressedHandler(pressedHandler);
  handThree.setReleasedHandler(releasedHandler);
}

void setupLED() {
  Serial.print("LED Setup...");

  FastLED.setBrightness(100);

  FastLED.addLeds<WS2811, LED_DATA_PIN, RGB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  fill_rainbow(leds, NUM_LEDS, 0, 7);
  FastLED.show();
  Serial.println("...Complete!");
}

void setupScale() {

  Serial.print("Scale =  ");
  sm.setCurrentScale(MAJOR);
  sm.setFundamental(48);

  Serial.print(sm.getFundamentalName());
  Serial.print("    ");
  Serial.println(sm.getScaleName());
}

void checkButtons() {
  playButton.loop();
  tempoDownButton.loop();
  tempoUpButton.loop();
  handOne.loop();
  handTwo.loop();
  handThree.loop();
}

void checkKnobs() {
  float fraction = 0.0009708737864;
  int reading[3];
  for (int i = 0; i < 3; i++) {
    reading[i] = analogRead(DRUM_CONTROL_PIN[i]);
    sampleMixer.gain(2 - i, reading[i] * fraction);
    // update leds[0] with the HSV result
  }
  CHSV redFade = rgb2hsv_approximate(CRGB::Red);  // to use the current color of leds[0]
  redFade.v = reading[2] / 4;                     // set the brightness 0 -> 100
  drumFaderLED[2] = redFade;

  CHSV orangeFade = rgb2hsv_approximate(CRGB::Orange);  // to use the current color of leds[0]
  orangeFade.v = reading[0] / 4;                        // set the brightness 0 -> 100
  drumFaderLED[0] = orangeFade;

  CHSV blueFade = rgb2hsv_approximate(CRGB::BlueViolet);  // to use the current color of leds[0]
  blueFade.v = reading[1] / 4;                            // set the brightness 0 -> 100
  drumFaderLED[1] = blueFade;

  // for (int j = 0; j < 8; j++) {
  //   CHSV newTargetHSV = rgb2hsv_approximate(kickSequenceLED[j]);
  //   newTargetHSV.v = reading[0] / 4;
  //   kickSequenceLED[j] = newTargetHSV;
  // }

  // for (int j = 0; j < 8; j++) {
  //   CHSV newTargetHSV = rgb2hsv_approximate(snareSequenceLED[j]);
  //   newTargetHSV.v = reading[1] / 4;
  //   snareSequenceLED[j] = newTargetHSV;
  // }

  // for (int j = 0; j < 8; j++) {
  //   CHSV newTargetHSV = rgb2hsv_approximate(hatSequenceLED[j]);
  //   newTargetHSV.v = reading[2] / 4;
  //   hatSequenceLED[j] = newTargetHSV;
  // }
}


void updateTouch() {

  const int NUM_ELECTRODES = 12;

  currtouched5A = cap5A.touched();
  currtouched5B = cap5B.touched();
  currtouched5C = cap5C.touched();
  currtouched5D = cap5D.touched();

  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {

    if ((currtouched5A & _BV(i)) && !(lasttouched5A & _BV(i))) touchNoteOn(i);
    if ((currtouched5B & _BV(i)) && !(lasttouched5B & _BV(i))) touchNoteOn(i + 12);
    if ((currtouched5C & _BV(i)) && !(lasttouched5C & _BV(i))) touchNoteOn(i + 24);
    if ((currtouched5D & _BV(i)) && !(lasttouched5D & _BV(i))) touchNoteOn(i + 36);

    if (!(currtouched5A & _BV(i)) && (lasttouched5A & _BV(i))) touchNoteOff(i);
    if (!(currtouched5B & _BV(i)) && (lasttouched5B & _BV(i))) touchNoteOff(i + 12);
    if (!(currtouched5C & _BV(i)) && (lasttouched5C & _BV(i))) touchNoteOff(i + 24);
    if (!(currtouched5D & _BV(i)) && (lasttouched5D & _BV(i))) touchNoteOff(i + 36);
  }

  lasttouched5A = currtouched5A;
  lasttouched5B = currtouched5B;
  lasttouched5C = currtouched5C;
  lasttouched5D = currtouched5D;
}

void touchNoteOn(int keyNumber) {
  int keyNoteNumber[45] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4, 5, 6, 7, 8, 9, 10, 11, 12, 6, 7, 8, 9, 10, 11, 12, 13, 7, 8, 9, 10, 11, 12, 13 };
  static int voiceIndex = 0;
  if (keyNumber > 45) return;

  Serial.print("Key ");
  Serial.print(voiceIndex);
  Serial.print(" Pressed   -  Frequency = ");
  Serial.println(sm.getScaleNoteFrequency(keyNumber));
  noteIsOn[keyNumber] = true;
  usbMIDI.sendNoteOn(sm.getScaleNote(keyNumber), 100, MELODY_CHANNEL);
  guitarString[voiceIndex]->noteOn(sm.getScaleNoteFrequency(keyNoteNumber[keyNumber]), 1);

  voiceIndex++;
  if (voiceIndex > 7) voiceIndex = 0;
}

void touchNoteOff(int keyNumber) {

  if (keyNumber > 45) return;

  Serial.print("Key ");
  Serial.print(keyNumber);
  Serial.println(" Released");
  noteIsOn[keyNumber] = false;
  usbMIDI.sendNoteOff(sm.getScaleNote(keyNumber), 0, MELODY_CHANNEL);
  // synth.noteOff(sm.getScaleNote(noteIndex[i]), 100);
}

void buttonHandler(Button2& btn) {
  switch (btn.getType()) {
    case single_click:
      break;
    case double_click:
      Serial.print("double ");
      break;
    case triple_click:
      Serial.print("triple ");
      break;
    case long_click:
      Serial.print("long");
      break;
    default:
      Serial.print(btn.getType());
      break;
  }
  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}

void pressedHandler(Button2& btn) {
  if (btn == playButton) {
    Serial.print("Play");
    sequencer.togglePlay();
  }
  if (btn == tempoDownButton) {
    Serial.print("Tempo Down ");
    sequencer.decreaseTempo();
  }
  if (btn == tempoUpButton) {
    Serial.print("Tempo Up ");
    sequencer.increaseTempo();
  }
  if (btn == handOne) {
    Serial.print("Hand One");
    handOn[0] = true;
    handHandler();
    smallHandLED[0] = CRGB::DeepPink;
    bigHandLED[2] = CRGB::DeepPink;
    usbMIDI.sendNoteOn(sm.getScaleNote(8), 100, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(10), 80, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(12), 80, BASS_CHANNEL);
  }
  if (btn == handTwo) {
    Serial.print("Hand Two");
    handOn[1] = true;
    handHandler();
    smallHandLED[1] = CRGB::DarkOrange;
    bigHandLED[1] = CRGB::DarkOrange;
    usbMIDI.sendNoteOn(sm.getScaleNote(12), 100, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(14), 80, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(8), 80, BASS_CHANNEL);
  }
  if (btn == handThree) {
    Serial.print("Hand Three");
    handOn[2] = true;
    handHandler();
    smallHandLED[2] = CRGB::YellowGreen;
    bigHandLED[0] = CRGB::YellowGreen;
    usbMIDI.sendNoteOn(sm.getScaleNote(13), 100, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(15), 80, BASS_CHANNEL);
    usbMIDI.sendNoteOn(sm.getScaleNote(10), 80, BASS_CHANNEL);
  }
  Serial.println(" was pressed!");
}

void releasedHandler(Button2& btn) {
  if (btn == playButton) {
    Serial.print("Play");
  }
  if (btn == tempoDownButton) {
    Serial.print("Tempo Down ");
  }
  if (btn == tempoUpButton) {
    Serial.print("Tempo Up ");
  }
  if (btn == handOne) {
    Serial.print("Hand One");
    handOn[0] = false;
    handHandler();
    smallHandLED[0] = CRGB::Black;
    bigHandLED[2] = CRGB::Black;
    usbMIDI.sendNoteOff(sm.getScaleNote(8), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(10), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(12), 0, BASS_CHANNEL);
  }
  if (btn == handTwo) {
    Serial.print("Hand Two");
    handOn[1] = false;
    handHandler();
    smallHandLED[1] = CRGB::Black;
    bigHandLED[1] = CRGB::Black;
    usbMIDI.sendNoteOff(sm.getScaleNote(12), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(14), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(8), 0, BASS_CHANNEL);
  }
  if (btn == handThree) {
    Serial.print("Hand Three");
    handOn[2] = false;
    handHandler();
    smallHandLED[2] = CRGB::Black;
    bigHandLED[0] = CRGB::Black;
    usbMIDI.sendNoteOff(sm.getScaleNote(13), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(15), 0, BASS_CHANNEL);
    usbMIDI.sendNoteOff(sm.getScaleNote(10), 0, BASS_CHANNEL);
  }
  Serial.println(" was released!");
}

void updateSequencerLEDS() {
  for (uint8_t i = 0; i < 8; i++) {
    if (bitRead(sequencer.currentKickSequence, i)) {
      kickSequenceLED[i] = CRGB::Red;
      if (sequencer.beatIndex == i) kickSequenceLED[i] = CRGB::Crimson;
    } else {
      kickSequenceLED[i] = CRGB::Black;
      if (sequencer.beatIndex == i) kickSequenceLED[i] = CRGB::Gray;
    }
    if (bitRead(sequencer.currentHatSequence, i)) {
      hatSequenceLED[i] = CRGB::Cyan;
      if (sequencer.beatIndex == i) hatSequenceLED[i] = CRGB::Blue;
    } else {
      hatSequenceLED[i] = CRGB::Black;
      if (sequencer.beatIndex == i) hatSequenceLED[i] = CRGB::Gray;
    }
    if (bitRead(sequencer.currentSnareSequence, i)) {
      snareSequenceLED[i] = CRGB::HotPink;
      if (sequencer.beatIndex == i) snareSequenceLED[i] = CRGB::Pink;
    } else {
      snareSequenceLED[i] = CRGB::Black;
      if (sequencer.beatIndex == i) snareSequenceLED[i] = CRGB::Gray;
    }
  }
}

void handHandler() {
  static bool currentHandState = false;
  static bool previousHandState = false;

  previousHandState = currentHandState;
  currentHandState = true;

  if (handOn[0] & !handOn[1] & !handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(0) - 12));
  } else if (!handOn[0] && handOn[1] && !handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(2) - 12));
  } else if (!handOn[0] && !handOn[1] && handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(4) - 12));
  } else if (handOn[0] && handOn[1] && !handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(1) - 12));
  } else if (!handOn[0] && handOn[1] && handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(3) - 12));
  } else if (handOn[0] && handOn[1] && !handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(5) - 12));
  } else if (handOn[0] && handOn[1] && handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(6) - 12));
  } else if (handOn[0] && !handOn[1] && handOn[2]) {
    handSaw.frequency(sm.getFrequency(sm.getScaleNote(7) - 12));
  } else {

    handSawEnvelope.noteOff();
    currentHandState = false;
  }

  if (previousHandState != currentHandState && currentHandState == true) handSawEnvelope.noteOn();
}

void readAnalogInputs() {
  // Serial.print("Pattern Controls: ");
  for (int i = 0; i < 3; i++) {
    // int reading = analogRead(PATTERN_CONTROL_PIN[i]);
    // Serial.print(i);
    // Serial.print(" - ");
    // Serial.print(reading);
    // Serial.print("  ");
  }
  // Serial.print("Drum Controls: ");
  for (int i = 0; i < 3; i++) {
    // int reading = analogRead(DRUM_CONTROL_PIN[i]);
    // Serial.print(i);
    // Serial.print(" - ");
    // Serial.print(reading);
    // Serial.print("  ");
  }
  // Serial.println();
}

void audioSetup() {

  AudioMemory(300);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  instrumentMixer.gain(0, 0.9);  //drums
  instrumentMixer.gain(1, 0.8);  //hands
  instrumentMixer.gain(2, 0.1);  //robots
  instrumentMixer.gain(3, 0.5);  //keys

  drumMachineSetup();
  handSawSetup();
  voiceSetup();
  stringSetup();
}

void stringSetup() {
  stringMixerOne.gain(0, 0.3);
  stringMixerOne.gain(1, 0.3);
  stringMixerOne.gain(2, 0.3);
  stringMixerOne.gain(3, 0.3);
  stringMixerTwo.gain(0, 0.3);
  stringMixerTwo.gain(1, 0.3);
  stringMixerTwo.gain(2, 0.3);
  stringMixerTwo.gain(3, 0.3);
  stringMixerThree.gain(0, 0.5);
  stringMixerThree.gain(1, 0.5);
  stringMixerThree.gain(2, 0.5);
  stringMixerThree.gain(3, 0.5);
}

void drumMachineSetup() {

  sampleMixer.gain(0, 0.5);  // kick
  sampleMixer.gain(1, 0.5);  // snare
  sampleMixer.gain(2, 0.5);  //hat
  sampleMixer.gain(2, 0.3);  //handbass
  drumFilter.resonance(0.9);
  drumFilter.inputDrive(1);
  drumFilter.interpolationMethod(LADDER_FILTER_INTERPOLATION_FIR_POLY);

  drumverb.reverbTime(1);

  return true;
}

void handSawSetup() {
  handSaw.begin(1, 100, WAVEFORM_BANDLIMIT_PULSE);
  handSawEnvelope.attack(1);
  handSawEnvelope.decay(50);
  handSawEnvelope.sustain(0.1);
  handSawEnvelope.release(30);
}

void voiceSetup() {

  sawVoice.begin(1, sm.getScaleNoteFrequency(7), WAVEFORM_SAWTOOTH_REVERSE);
  sawVoice.frequencyModulation(0.01);
  noise.amplitude(0.0);
  noiseFilter.frequency(6000);
  noiseFilter.resonance(0.7);

  voiceMixer.gain(0, 0.3);
  voiceMixer.gain(1, 0.1);

  voiceEnvelope.attack(100);
  voiceEnvelope.sustain(1);
  voiceEnvelope.release(100);

  vocalLfo.begin(1, 5, WAVEFORM_SINE);

  pinkNoise.amplitude(0);
  vibratoAmount.gain(0, 0.2);

  amplitudeModulation.gain(0.2);

  float resonance = 3;
  // float drive = 3;
  // formantFilter[0].inputDrive(drive);
  // formantFilter[1].inputDrive(drive);
  // formantFilter[2].inputDrive(drive);

  formantFilter[0].resonance(resonance);
  formantFilter[1].resonance(resonance);
  formantFilter[2].resonance(resonance);
  formantFilter[3].resonance(resonance);
  formantFilter[4].resonance(resonance);
  formantFilter[5].resonance(resonance);
  formantFilter[6].resonance(resonance);
  formantFilter[7].resonance(resonance);

  formantMixer.gain(0, 0.2);
  formantMixer.gain(1, 0.2);
  formantMixer.gain(2, 0.2);
  formantMixer.gain(3, 0.2);

  // bitCrusher.bits(16);
}

void setFormant(int value, Vowel start, Vowel end) {

  if (value > 20) value = 20;
  if (value < 1) value = 1;

  formantFilter[0].frequency(map(value, 1, 20, start.frequency[0], end.frequency[0]));
  formantFilter[1].frequency(map(value, 1, 20, start.frequency[1], end.frequency[1]));
  formantFilter[2].frequency(map(value, 1, 20, start.frequency[2], end.frequency[2]));
  formantFilter[3].frequency(map(value, 1, 20, start.frequency[3], end.frequency[3]));

  formantFilter[4].frequency(map(value, 1, 20, start.frequency[0], end.frequency[0]));
  formantFilter[5].frequency(map(value, 1, 20, start.frequency[1], end.frequency[1]));
  formantFilter[6].frequency(map(value, 1, 20, start.frequency[2], end.frequency[2]));
  formantFilter[7].frequency(map(value, 1, 20, start.frequency[3], end.frequency[3]));

  formantFilter[0].resonance(mapf(value, 1, 20, start.resonance[0], end.resonance[0]));
  formantFilter[1].resonance(mapf(value, 1, 20, start.resonance[1], end.resonance[1]));
  formantFilter[2].resonance(mapf(value, 1, 20, start.resonance[2], end.resonance[2]));
  formantFilter[3].resonance(mapf(value, 1, 20, start.resonance[3], end.resonance[3]));
  formantFilter[4].resonance(mapf(value, 1, 20, start.resonance[0], end.resonance[0]));
  formantFilter[5].resonance(mapf(value, 1, 20, start.resonance[1], end.resonance[1]));
  formantFilter[6].resonance(mapf(value, 1, 20, start.resonance[2], end.resonance[2]));
  formantFilter[7].resonance(mapf(value, 1, 20, start.resonance[3], end.resonance[3]));

  formantMixer.gain(0, mapf(value, 1, 20, start.gain[0], end.gain[0]));
  formantMixer.gain(1, mapf(value, 1, 20, start.gain[1], end.gain[1]));
  // formantMixer.gain(2, mapf(value, 1, 20, start.gain[2], end.gain[2]));
  // formantMixer.gain(3, mapf(value, 1, 20, start.gain[3], end.gain[3]));
}

void updateStringLEDs() {
  int readings[9];
  if (stringFFT.available()) {
    readings[1] = stringFFT.read(0, 5) * 5000;
    readings[2] = stringFFT.read(0, 5) * 5000;
    readings[0] = stringFFT.read(2, 8) * 5000;
    readings[3] = stringFFT.read(2, 8) * 5000;
    readings[4] = stringFFT.read(4, 10) * 5000;
    readings[5] = stringFFT.read(2, 8) * 5000;
    readings[6] = stringFFT.read(4, 10) * 5000;
    readings[7] = stringFFT.read(8, 20) * 5000;
    readings[8] = stringFFT.read(8, 20) * 5000;
    readings[9] = stringFFT.read(91, 100) * 5000;

    for (int i = 0; i < 9; i++) {
      leds[i + 31].setHSV(readings[i], readings[i], readings[i]);
      // Serial.print(readings[i]);
      // Serial.print("  ");
    }
    // Serial.println();
  }
}
// int getAftertouch() {
//   int numberOfNotesPlaying = 0;
//   int touchTotal = 0;
//   int averageTouch = 0;
//   int aftertouch = 0;
//   for (byte i = 0; i < 12; i++) {
//     filteredReading[i] = cap5C.filteredData(i);
//     filteredReading[i + 12] = cap5A.filteredData(i);
//   }

//   for (byte i = 0; i < 24; i++) {
//     if (noteIsOn[i]) {
//       numberOfNotesPlaying++;
//       touchTotal += filteredReading[i];
//     }
//   }

//   averageTouch = touchTotal / numberOfNotesPlaying;

//   if (numberOfNotesPlaying > 0) {
//     if (numberOfNotesPlaying == 1) {
//       aftertouch = map(averageTouch, 45, 65, 127, 0);
//     } else {
//       aftertouch = map(averageTouch, 20, 45, 127, 0);
//     }

//     aftertouch = constrain(aftertouch, 0, 127);

//     Serial.print("Aftertouch = ");
//     Serial.println(aftertouch);
//   }

//   return aftertouch;
// }
