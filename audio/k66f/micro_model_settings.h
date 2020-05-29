#ifndef MICRO_MODEL_SETTINGS_H_
#define MICRO_MODEL_SETTINGS_H_

constexpr int kAudioSampleFrequency = 16000;

// The size of the input time series data we pass to the FFT to produce the
// frequency information. This has to be a power of two, and since we're dealing
// To capture  30ms of 16KHz inputs, we need 480 samples; 512  is the next power of 2.
constexpr int kMaxAudioSampleSize = 512;  // only 30 ms

// TODO : - to capture 1.2 sec we need to have ~512*40 sampe size,
//constexpr int kMaxAudioSampleSize = 512*32;   - this is the goal: to capture 1.2 secs

#endif  // MICRO_MODEL_SETTINGS_H_
