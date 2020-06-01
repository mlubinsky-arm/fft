/***
To capture 30ms of data at 16KHz: 16*3 = 480 samples;
The reserved buffer size is 512: (in file ./audio/k66f/micro_model_settings.h)
FFT requierements - input FFT array to be the power of 2
In this app we need to capture for FFT ~1.2 sec of data (30ms * 40),
 The max size of FFT input in CMSIS DSP  4096.
 How to pass to CMSIS FFT the array of the bigger size?
***/

#include "audio_provider.h"

#define ARM_MATH_CM4
#include "arm_math.h"
#include "arm_const_structs.h"

#include "mbed.h"

// Files for testing FFT:
#include "i500Hz_2048points.h"
#include "i250Hz_2048points.h"

void cmsis_fft(int* data, int data_size, char* name);

void audioFFT() { // to be called in infinite loop
  int start_ms=0; // TODO how to read the new data? Do we need to calculate: start_ms += 30*4 ?
  constexpr int kFeatureSliceDurationMs = 30 ;
  // line above is cross-related with variables  in audio_privider.cc:
  // 1) kAudioSampleFrequency = 16000  Hz  (1600*30/1000)=480 data points for 30ms
  // 2) int kNoOfSamples = 512;
  // 3) kMaxAudioSampleSize = 512;

  // To capture samples  ~1.2 sec of data (30ms * 40) we need to have N_BUFFERS 32 or 64:
  #define N_BUFFERS 4
  // 5 * 512 = 2048 data points
  // if we make N_BUFFERS 32 or 64 we can capture > 1 sec of data, but ..
  // .. how to feed it to CMSIS FFT which can handle only 2048 or 4096 samples max?
  #define BIG_SIZE (512 * N_BUFFERS)
  //int16_t big_audio_samples[BIG_SIZE];
  int  big_audio_samples[BIG_SIZE];

  for (int i=0; i < BIG_SIZE; i++){
    big_audio_samples[i]=0;
  }
  int big_index=0;
  // to grab ~1 sec of data N_BUFFERS should be ~32 or 64
  int32_t latest_audio_time_ms=0;
  static int32_t previous_audio_time_ms=0;
  for (int k=0; k <  N_BUFFERS; k++ ){ // calling  GetAudioSamples() in the loop
    int16_t* audio_samples = nullptr;
    int audio_samples_size = 0;
/**/
    while  (1){ // check what new 30ms chunk of audio data in buffer
      latest_audio_time_ms = LatestAudioTimestamp();
      //printf("\n latest_audio_time_ms =%ld  previous_audio_time_ms=%ld kFeatureSliceDurationMs=%d", latest_audio_time_ms,  previous_audio_time_ms, kFeatureSliceDurationMs);
      if (latest_audio_time_ms == 0 || (latest_audio_time_ms -  previous_audio_time_ms > kFeatureSliceDurationMs) ){
        previous_audio_time_ms = latest_audio_time_ms;
        //printf("\n break");
        break;
      }
      printf("\n wait");
      ThisThread::sleep_for(kFeatureSliceDurationMs);
    }
/**/

    printf("\n -------- before calling GetAudioSamples() k=%d", k);
    GetAudioSamples(  // one call GetAudioSamples() gives 30ms of data only
                   start_ms + k * kFeatureSliceDurationMs, // TODO
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);
    printf("\n -------- after calling GetAudioSamples()  audio_samples_size=%d", audio_samples_size);
    int m2;
    for (int m=0; m<10000; m++){
         m2=m/2+m;
    }
    printf("\n m2=%d",m2);
 //  GetAudioSamples() usually returns 512 samples,  but the samples with index [480 - 511] are filled with 0
 //  TODO: is it OK to pass the samples with index [480 - 511] to FFT or  it is better do not pass it to FFT?

    // copy data to the bigger array
    for ( int j=0; j < audio_samples_size; j++){
      if (big_index >= BIG_SIZE) break;
      big_audio_samples[big_index] = audio_samples[j];
      big_index++;
    }
  }  // end of calling  GetAudioSamples() in the loop
  printf("\n -------- after LOOP GetAudioSamples() big_index=%d", big_index);
  if (big_index != BIG_SIZE){
    printf("\n -------- ERROR big_index=%d (BIG_SIZE)-1=%d",  big_index, (BIG_SIZE-1));
  }
  int  fft_samples[BIG_SIZE];
  for (int i=0; i < big_index; i++){
    fft_samples[i] = big_audio_samples[i];
  }
  // This is to have exactly 2048 elements in array
  for (int i=big_index; i < BIG_SIZE; i++){
    fft_samples[i] = 0;
  }

  printf("\n -------- before calling cmsis_fft");
  cmsis_fft( fft_samples, 2048, (char *)(" FFT board "));
  return;
}
/*

// find the peaks for the specific frequencies range (~250Hz and ~500Hz):
    float   mpeak250 = 0.0;
    float   ipeak250 = 0.0;
    float   mpeak500 = 0.0;
    float   ipeak500 = 0.0;
    float freq;
    float m2;

    for (int i=0; i < FFT_LEN; i++){
     printf("\n -- %d",i);
     freq = 1.0 * i * 1600 / FFT_LEN;  // TODO  - is it correct?
     if (freq >= 240.0 && freq <= 260.0){
         m2 =  magn[i] * magn[i];
         if  ( m2 > mpeak250 ){
             ipeak250 = freq;
             mpeak250 = m2;
         }
     }
     if (freq >= 480.0 && freq <= 520.0){
         m2 =  magn[i] * magn[i];
         if  ( m2 > mpeak500 ){
             ipeak500 = freq;
             mpeak500 = m2;
         }
     }  // end for loop for finding peaks
   }

  printf("\n ipeak250=%f mpeak250=%f", ipeak250, mpeak250);
  printf("\n ipeak500=%f mpeak500=%f", ipeak500, mpeak500);
  if (mpeak250 > 9000.0  && mpeak500 > 4000.0){
       printf("\n --- FOUND mpeak250 > 9000.0  && mpeak500 > 4000.0 --- ");
  }
*/



void cmsis_fft(int* data, int data_size, char* name)
{
  #define FFT_SIZE 2048

  //printf("\n name=%s  size=%d", name, data_size);
  if (data_size != FFT_SIZE){
    printf ("Error data_size=%d != FFT_SIZE=%d",data_size , FFT_SIZE);
    return;
  }


  static arm_rfft_instance_q15 fft_instance;
  static q15_t output[FFT_SIZE*2]; //has to be twice FFT size
  arm_status status = arm_rfft_init_q15(
         &fft_instance,
         FFT_SIZE, // bin count
         0, // forward FFT
         1 // output bit order is normal
  );

  if (status != 0){
    printf( "FFT init status= %d\n", status);
    return;
  }
  printf("\n Before arm_rfft_q15()");

  arm_rfft_q15(&fft_instance, (q15_t*)data, output);

  arm_abs_q15(output, output, FFT_SIZE*2);

  printf( "\n AFTER arm_abs_q15 ");

  for (int i=0; i < data_size; i++) {
    if  (output[i] > 1){
     printf("\n i=%d output= %d freq: i*(16000/FFT_SIZE)=%d", i, output[i], i*16000/FFT_SIZE);
    }
  }

}

int main(void)
{
  // Test CMSIS FFT:
  // cmsis_fft(i500, 2048 , (char *)(" integer 500Hz "));
  // cmsis_fft(i250, 2048 , (char *)(" integer 250Hz "));

  int i=0;
  while (1) {
     printf("\n %d ", i++);
     if (i > 999999) i=0;
     audioFFT();
  }

}
