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

// Files for testing FFT:
#include "i500Hz_2048points.h"
#include "i250Hz_2048points.h"
//#include "iSound.h"

#define FFT_SIZE 2048
void cmsis_fft(int* data, int data_size);

void audioFFT() {

    int  fft_samples[FFT_SIZE];
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
    for (int k=0; k <4; k++) {  //4 *512 = 2048 data points
      int16_t* audio_samples = nullptr;
      int audio_samples_size = 0;

      GetAudioSamples(  // one call GetAudioSamples() gives 30ms of audio = 512 data points @16000 Hz
                   start_ms + k * kFeatureSliceDurationMs, // TODO
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);

      for ( int j=0; j < audio_samples_size; j++){
        if (big_index >= FFT_SIZE) break;
        fft_samples[big_index] = audio_samples[j];
        big_index++;
      }
    }

      if (big_index == FFT_SIZE) {
          printf("\n -big_index == FFT_SIZE before calling cmsis_fft");
          cmsis_fft( fft_samples, FFT_SIZE);
          printf("\n -------- after calling cmsis_fft");
      }
}

void cmsis_fft(int* data, int data_size)
{
  if (data_size != FFT_SIZE){
    printf ("Error data_size=%d != FFT_SIZE=%d",data_size , FFT_SIZE);
    return;
  }
  printf("\n INSIDE cmsis_fft ");

  static arm_rfft_instance_q15 fft_instance;
  static q15_t output[FFT_SIZE*2]; //has to be twice FFT size
  arm_status status = arm_rfft_init_q15(
         &fft_instance,
         FFT_SIZE, // bin count
         0, // forward FFT
         1 // output bit order is normal
  );
  printf( "FFT init status= %d\n", status);
  if (status != 0){
    return;
  }

  printf("\n Before arm_rfft_q15()");

  arm_rfft_q15(&fft_instance, (q15_t*)data, output);
  printf( "\n AFTER arm_rmfft_q15 ");
  return;
  arm_abs_q15(output, output, FFT_SIZE*2);

  printf( "\n AFTER arm_abs_q15 ");

  for (int i=0; i < data_size; i++) {
    if  (output[i] > 2){
     printf("\n i=%d output= %d freq: i*(16000/FFT_SIZE)=%d", i, output[i], i*16000/FFT_SIZE);
    }
  }
   printf("\n =====  END OF FFT =========");
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
