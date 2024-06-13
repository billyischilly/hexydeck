#include "filter_variable.h"
#include "mixer.h"
#include "synth_whitenoise.h"
#include <Audio.h>

//Drums
AudioPlayMemory kickSample;
AudioPlayMemory snareSample;
AudioPlayMemory hatSample;

AudioMixer4 sampleMixer;

AudioFilterLadder drumFilter;
AudioEffectReverb drumverb;

// Hands
AudioSynthWaveformModulated handSaw;

AudioEffectEnvelope handSawEnvelope;

//Robot Voices
AudioSynthWaveform vocalLfo;
AudioSynthNoisePink pinkNoise;

AudioMixer4 vibratoAmount;

AudioSynthWaveformModulated sawVoice;
AudioSynthNoiseWhite noise;
AudioFilterStateVariable noiseFilter;

AudioMixer4 voiceMixer;

AudioAmplifier amplitudeModulation;
AudioEffectMultiply multiplier;

AudioFilterStateVariable formantFilter[8];
AudioMixer4 formantMixer;
AudioEffectEnvelope voiceEnvelope;
AudioEffectBitcrusher bitCrusher;

AudioSynthKarplusStrong ks1;
AudioSynthKarplusStrong ks2;
AudioSynthKarplusStrong ks3;
AudioSynthKarplusStrong ks4;
AudioSynthKarplusStrong ks5;
AudioSynthKarplusStrong ks6;
AudioSynthKarplusStrong ks7;
AudioSynthKarplusStrong ks8;

AudioSynthKarplusStrong* guitarString[] = { &ks1, &ks2, &ks3, &ks4, &ks5, &ks6, &ks7, &ks8 };

AudioMixer4 stringMixerOne;
AudioMixer4 stringMixerTwo;
AudioMixer4 stringMixerThree;
AudioAnalyzeFFT256 stringFFT;
AudioMixer4 instrumentMixer;
AudioOutputI2S output;
AudioControlSGTL5000 sgtl5000_1;



// AudioSynthKarplusStrong string1;
// AudioSynthKarplusStrong string2;
// AudioSynthKarplusStrong string3;
// AudioSynthKarplusStrong string4;
// AudioSynthKarplusStrong string5;
// AudioSynthKarplusStrong string6;
// AudioSynthKarplusStrong string7;


AudioConnection patchCo1(ks1, 0, stringMixerOne, 0);
AudioConnection patchCo2(ks2, 0, stringMixerOne, 1);
AudioConnection patchCo3(ks3, 0, stringMixerOne, 2);
AudioConnection patchCo4(ks4, 0, stringMixerOne, 3);
AudioConnection patchCo5(ks5, 0, stringMixerTwo, 0);
AudioConnection patchCo6(ks6, 0, stringMixerTwo, 1);
AudioConnection patchCo7(ks7, 0, stringMixerTwo, 2);
AudioConnection patchCo8(ks8, 0, stringMixerTwo, 3);
AudioConnection patchCo9(stringMixerOne, 0, stringMixerThree, 0);
AudioConnection patchCo10(stringMixerTwo, 0, stringMixerThree, 1);
AudioConnection patchCo11(stringMixerThree, 0, instrumentMixer, 3);
AudioConnection patchCo12(stringMixerThree, 0, stringFFT, 0);

AudioConnection patchCor1(vocalLfo, 0, vibratoAmount, 0);
AudioConnection patchCor2(pinkNoise, 0, vibratoAmount, 1);
AudioConnection patchCor3(vibratoAmount, 0, sawVoice, 0);
AudioConnection patchCor4(vibratoAmount, 0, amplitudeModulation, 0);
AudioConnection patchCor5(amplitudeModulation, 0, sawVoice, 1);

AudioConnection patchCor9(sawVoice, 0, voiceMixer, 0);
AudioConnection patchCor0(noise, 0, noiseFilter, 0);
AudioConnection patchCor14(noiseFilter, 2, voiceMixer, 1);


AudioConnection patchCorda(voiceMixer, formantFilter[0]);
AudioConnection patchCordb(voiceMixer, formantFilter[1]);
AudioConnection patchCordc(voiceMixer, formantFilter[2]);
AudioConnection patchCordp(voiceMixer, formantFilter[3]);

// AudioConnection patchCordd(formantFilter[0], 2, formantMixer, 0);
AudioConnection patchCordh(formantFilter[0], 1, formantFilter[4], 0);
AudioConnection patchCorde(formantFilter[1], 1, formantFilter[5], 0);
AudioConnection patchCordf(formantFilter[2], 1, formantFilter[6], 0);
AudioConnection patchCordo(formantFilter[3], 1, formantFilter[7], 0);

AudioConnection patchCordh1(formantFilter[4], 2, formantMixer, 0);
AudioConnection patchCorde1(formantFilter[5], 0, formantMixer, 1);
AudioConnection patchCordf1(formantFilter[6], 0, formantMixer, 2);
AudioConnection patchCordo1(formantFilter[7], 0, formantMixer, 3);

AudioConnection patchCorh(formantMixer, 0, voiceEnvelope, 0);
AudioConnection patchCordi(voiceEnvelope, 0, instrumentMixer, 2);

AudioConnection patchCord0(handSaw, handSawEnvelope);
AudioConnection patchCordq(handSawEnvelope, 0, sampleMixer, 3);

AudioConnection patchCord1(kickSample, 0, sampleMixer, 0);
AudioConnection patchCord2(snareSample, 0, sampleMixer, 1);
AudioConnection patchCord3(hatSample, 0, sampleMixer, 2);

AudioConnection patchCord4(sampleMixer, 0, drumFilter, 0);
AudioConnection patchCord5(drumFilter, 0, instrumentMixer, 0);
AudioConnection patchCord6(drumFilter, 0, drumverb, 0);
AudioConnection patchCord7(drumverb, 0, instrumentMixer, 1);
AudioConnection patchCord8(instrumentMixer, 0, output, 0);
AudioConnection patchCord9(instrumentMixer, 0, output, 1);