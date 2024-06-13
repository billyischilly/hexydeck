#include <NewPing.h>
#include <RunningMedian.h>

#define SONAR_NUM 5       // Number of sensors.
#define MAX_DISTANCE 25   // Maximum distance (in cm) to ping.
#define THRESHOLD 25      //
#define PING_INTERVAL 15  // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

#define PINGS_IN_ARRAY 7

#define NOTE_ON_QUANTILE 0.5  // To give fast response and reduce retriggering of notes, configure a lower noteOn quantile than noteOff. Median is 0.5.
#define NOTE_OFF_QUANTILE 0.5

RunningMedian distanceReadings[NUMBER_OF_SENSORS] = {
  RunningMedian(PINGS_IN_ARRAY),
  RunningMedian(PINGS_IN_ARRAY),
  RunningMedian(PINGS_IN_ARRAY),
  RunningMedian(PINGS_IN_ARRAY),
  RunningMedian(PINGS_IN_ARRAY)
};
RunningMedian y = RunningMedian(20);

NewPing sonar[SONAR_NUM] = {    // Sensor object array.
  NewPing(5, 4, MAX_DISTANCE),  // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(30, 29, MAX_DISTANCE),
  NewPing(3, 2, MAX_DISTANCE),
  NewPing(1, 0, MAX_DISTANCE),
  NewPing(33, 34, MAX_DISTANCE)
};

unsigned long pingTimer;  // Holds the times when the next ping should happen for each sensor.
int cm[SONAR_NUM];        // Where the ping distances are stored.

bool inRange[SONAR_NUM - 1] = { false, false, false, false };  //
uint8_t currentSensor = 0;                                     // Keeps track of which sensor is active.
uint8_t previousSensor = 0;
uint8_t value[SONAR_NUM];

int x = 0;
// int y = THRESHOLD;
int previousPosition = 0;

int currentNote = 0;

CRGB robotColors[4] = { CRGB::Crimson, CRGB::BlueViolet, CRGB::DarkOrange, CRGB::ForestGreen };

void setupSonar() {
  pingTimer = millis() + 75;  // First ping starts at 75ms, gives time for the Arduino to chill before starting.
}

// // Declare variables
float currentPitch = 0;
float targetPitch = 0;
int glideTime = 4;  // Adjust this value to control the speed of the portamento
unsigned long lastUpdateTime = 0;
bool isPortamentoActive = false;

float portamento(int midiNote) {

  // Convert the MIDI note number to a frequency value
  float frequency = pow(2.0, (midiNote - 69) / 12.0) * 440.0;

  // If the frequency has changed, set the target frequency and begin portamento
  if (frequency != currentPitch) {
    targetPitch = frequency;
    isPortamentoActive = true;
  }

  bool isAscending = targetPitch > currentPitch;

  // If portamento is active, update the frequency
  if (isPortamentoActive) {
    // Check if it's time to update the frequency
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= glideTime) {
      // Calculate the frequency difference and amount to glide
      float frequencyDiff = abs(targetPitch - currentPitch);
      float glideAmount = frequencyDiff / glideTime;
      // Calculate the next frequency value

      if (isAscending) {
        currentPitch = currentPitch + glideAmount;
      } else {
        currentPitch = currentPitch - glideAmount;
      }
      // // // Limit the frequency to the valid range (20-20000 Hz for most synths)
      // int nextFrequency = max(20.0f, min(nextFrequency, 20000.0f));
      // // // Convert the frequency back to a MIDI note number
      // float nextNote = 12.0 * log(nextFrequency / 440.0) / log(2.0) + 69;
      // // // Round the note to the nearest integer
      // nextNote = round(nextNote);
      // // Update the current frequency and note values
      // currentPitch = nextFrequency;
      // currentNote = nextNote;
      // // Update the last update time
      lastUpdateTime = currentTime;
      // Check if we've reached the target frequency
      if (isAscending && currentPitch > targetPitch) currentPitch = targetPitch;
      if (!isAscending && currentPitch < targetPitch) currentPitch = targetPitch;

      if (currentPitch == targetPitch) {
        isPortamentoActive = false;
      }
    }
  }

  return currentPitch;
}


// // Declare variables
// int currentPitch = 0;
// int targetPitch = 0;
// int glideTime = 4;  // Adjust this value to control the speed of the portamento
// unsigned long lastUpdateTime = 0;
// bool isPortamentoActive = false;

// void portamento(int pitch) {

//   // If the pitch has changed, set the target pitch and begin portamento
//   if (pitch != currentPitch) {
//     targetPitch = pitch;
//     isPortamentoActive = true;
//   }

//   bool isAscending = targetPitch > currentPitch;

//   // If portamento is active, update the pitch
//   if (isPortamentoActive) {
//     // Check if it's time to update the pitch
//     unsigned long currentTime = millis();
//     if (currentTime - lastUpdateTime >= glideTime) {
//       // Calculate the pitch difference and amount to glide
//       int pitchDiff = abs(targetPitch - currentPitch);
//       int glideAmount = (float)pitchDiff / glideTime;
//       // Calculate the next pitch value
//       int nextPitch;
//       if (isAscending) {
//         nextPitch = currentPitch + glideAmount;
//       } else {
//         nextPitch = currentPitch - glideAmount;
//       }
//       // Limit the pitch to the valid range (0-255 for most Arduino synths)
//       // nextPitch = max(0, min(nextPitch, 255));
//       // Send the pitch value to the synth
//       // Update the current pitch value
//       currentPitch = nextPitch;
//       // Update the last update time
//       lastUpdateTime = currentTime;
//       // Check if we've reached the target pitch

//       if(isAscending && currentPitch > targetPitch) currentPitch = targetPitch;
//       if(!isAscending && currentPitch < targetPitch) currentPitch = targetPitch;

//       if (currentPitch == targetPitch) {
//         isPortamentoActive = false;
//       }
//     }
//   }
// }

void sonarUpdate() {

  if (millis() >= pingTimer) {
    for (uint8_t i = 0; i < SONAR_NUM; i++) {  // Loop through all the sensors.
      processPing(i);
    }
    pingTimer += PING_INTERVAL;

    // Determine current location of hand by finding which sensor has the lowest reading
    static RunningMedian horizontalPosition = RunningMedian(PINGS_IN_ARRAY);

    int currentLowest = THRESHOLD;
    currentSensor = 5;

    for (int i = 0; i < 4; i++) {
      if (distanceReadings[i].getMedian() < currentLowest) {
        currentLowest = distanceReadings[i].getMedian();
        previousSensor = currentSensor;
        currentSensor = i;
      }
    }

    horizontalPosition.add(currentSensor);

    x = horizontalPosition.getMedian();

    if (x != previousPosition && x < 5) {

      int scaleNoteNumber[4] = { 7, 8, 9, 10 };
      Serial.print("currentNote = ");
      currentNote = scaleNoteNumber[x];
      if (previousPosition == 5) voiceEnvelope.noteOn();
      Serial.println(currentNote);

    } else if (x != previousPosition && x >= 5) {

      voiceEnvelope.noteOff();
    }

    sonarLED[x] = robotColors[x];

    previousPosition = x;

    // portamento(currentNote);
    sawVoice.frequency(portamento(sm.getScaleNote(currentNote)));
    // Serial.print("currentPitch = ");
    // Serial.println(currentPitch);
    

    if (x < 5 && voiceEnvelope.isSustain()) {
      y.add(distanceReadings[x].getAverage());
      setFormant(y.getAverage(), a_alto, i_alto);
      vocalLfo.amplitude(1.2 - ((float)y.getAverage() * 0.05));
      noise.amplitude(((float)y.getAverage() * 0.05));
    }
  }
}

void processPing(uint8_t sensorNumber) {

  int reading = sonar[sensorNumber].ping_cm();
  if (reading == 0) reading = THRESHOLD + 10;  //Out of bounds readings are zero but work better as a high number.

  distanceReadings[sensorNumber].add(reading);

  // int noteOnValue = distanceReadings[sensorNumber].getQuantile(NOTE_ON_QUANTILE);
  // int noteOffValue = distanceReadings[sensorNumber].getQuantile(NOTE_OFF_QUANTILE);


  // if (cm[sensorNumber] < THRESHOLD && inRange[sensorNumber] == false) {

  //   inRange[sensorNumber] = true;

  // } else if (noteOffValue > THRESHOLD && inRange[sensorNumber] == true) {

  //   inRange[sensorNumber] = false;
  // }

  int filterReading = distanceReadings[4].getMedian();

  drumFilter.frequency(filterReading * filterReading * filterReading + 50);
  instrumentMixer.gain(1, 1 / filterReading);

  // int distance = constrain(medianDistance, 0, THRESHOLD);
  // uint8_t midiExpression = map(distance, THRESHOLD, 0, 0, 127);  // Sends relative distance as a scaled MIDI value
}
