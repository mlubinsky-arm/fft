/***
To capture 30ms of data at 16KHz: 16*3 = 480 samples
The reserved buffer size is 512
FFT requierements - input FFT array to be the power of 2
In this app we need to capture for FFT ~1.2 sec of data (30ms * 40),
 The max size of FFT input in CMSIS DSP  2048  or 4096.
***/

#include "audio_provider.h"

#define ARM_MATH_CM4
#include "arm_math.h"
#include "arm_const_structs.h"

#include "mbed.h"
//#include "platform/mbed_thread.h"

DigitalOut led(LED1);
// Files for testing CMSIS-FFT:

//#include "i500Hz_2048points.h"
//#include "i250Hz_2048points.h"
//#include "iSound.h"

//#define FFT_SIZE 2048
#define FFT_SIZE 1024
void cmsis_fft(int* data, int data_size);

void audioFFT() {

    static int  fft_samples[FFT_SIZE];
    int big_index=0;
    for (int i=0; i < FFT_SIZE; i++){
        fft_samples[i]=0;
    }

    int start_ms=0;
    constexpr int kFeatureSliceDurationMs = 30 ;
    // line above is cross-related with variables  in audio_privider.cc:
    // 1) kAudioSampleFrequency = 16000  Hz  (1600*30/1000)=480 data points for 30ms
    // 2) int kNoOfSamples = 512;
    // 3) kMaxAudioSampleSize = 512;
    int K_MAX=FFT_SIZE / 512;
    for (int k=0; k<K_MAX; k++) {  //4 *512 = 2048 data points
      int16_t* audio_samples = nullptr;
      int audio_samples_size = 0;

      GetAudioSamples(  // one call GetAudioSamples() gives 30ms of audio = 512 data points @16000 Hz
                   start_ms + k * kFeatureSliceDurationMs, // TODO
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);

      for ( int j=0; j < audio_samples_size; j++){
        if (big_index >= FFT_SIZE) break;
        fft_samples[big_index] = int (audio_samples[j]);
        big_index++;
      }
    }

      if (big_index == FFT_SIZE) {
          //printf("\n -------- before calling cmsis_fft");
          cmsis_fft( fft_samples, FFT_SIZE);
          //printf("\n -------- after calling cmsis_fft");
      }
}

void cmsis_fft(int* data, int data_size)
{
  if (data_size != FFT_SIZE){
    printf ("Error data_size=%d != FFT_SIZE=%d",data_size , FFT_SIZE);
    return;
  }

  static arm_rfft_instance_q15 fft_instance;
  static q15_t s[FFT_SIZE*2]; //has to be twice FFT size
  arm_status status = arm_rfft_init_q15(
         &fft_instance,
         FFT_SIZE, // bin count
         0, // forward FFT
         1 // output bit order is normal
  );

  if (status != 0){
    return;
  }

  arm_rfft_q15(&fft_instance, (q15_t*)data, s);
  arm_abs_q15(s, s, FFT_SIZE*2);

float Res = -1.60016261 -0.00092212 * s[30] + 0.0042692 * s[31] + 0.00153321* s[32] + 0.00034351* s[61] -0.00043756* s[62] -0.00045669* s[63];
float  lr = 1.0 / (1.0 + exp(-Res));
 
printf ("\n Res=%f   LR=%f", Res, lr);
led=1; //off

if (lr > 0.5) {
  printf ("\n ----------------Blink-----------------");
  led = 0; // on
  wait(0.5); //If you wish to wait (without sleeping), call 'wait_us'.
  //ThisThread::sleep_for(1000); //ms
  led=1; // off
}
 

/*
  float freq=0;
  for (int i=0; i < data_size; i++) {
    freq=  i*16000/FFT_SIZE;
    if  ( (freq >= 240.0 && freq <= 260.0) || (freq >= 480.0 && freq <= 520.0)){
     printf("\n i=%d  frequency=%.2f  signal= %d", i,  freq, s[i]);
    }
  }
*/
}

int main(void)
{
  // Test CMSIS FFT:
  //cmsis_fft(i500,  FFT_SIZE );
  //cmsis_fft(i250,  FFT_SIZE );
  //cmsis_fft(isound, FFT_SIZE );

  int i=0;
  while (1) {
     printf("\n %d ", i++);
     if (i > 999999) i=0;
     audioFFT();
  }
}
