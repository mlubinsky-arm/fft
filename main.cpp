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


#include "i500Hz_2048points.h"
#include "i250Hz_2048points.h"

//cut -f 1-2048 -d " " 250Hz.txt > 250_2048.txt
//cut -f 1-2048 -d " " 500Hz.txt > 500_2048.txt

void audioFFT() { // to be called in infinite loop
  int start_ms=0; // TODO how to read the new data? Do we need to calculate: start_ms += 30*4 ?
  constexpr int kFeatureSliceDurationMs = 30 ;
  // line above is cross-related to:
  // 1) int kNoOfSamples = 512; in audio_privider.cc
  // 2) kMaxAudioSampleSize = 512; in micro_model_settings.h

  // To capture samples  ~1.2 sec of data (30ms * 40) we need to have N_BUFFERS 32 or 64:
  #define N_BUFFERS 1
  // if we make N_BUFFERS 32 or 64 we can capture > 1 sec of data, but ..
  // .. how to feed it to CMSIS FFT which can handle only 4096 samples max?
  #define BIG_SIZE (512 * N_BUFFERS)
  int16_t big_audio_samples[BIG_SIZE];
  for (int i=0; i < BIG_SIZE; i++){
    big_audio_samples[i]=0;
  }
  int big_index=0;
  // to grab ~1 sec of data N_BUFFERS should be ~32 or 64
  for (int i=0; i <  N_BUFFERS; i++ ){
    int16_t* audio_samples = nullptr;
    int audio_samples_size = 0;
    GetAudioSamples(  // one call GetAudioSamples() gives 30ms of data only
                   start_ms + i * kFeatureSliceDurationMs, // TODO
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);
    if  (audio_samples_size > 480){    //  it usually returns 512
        audio_samples_size = 480;
    };
    // copy data to the bigger array
    for ( int j=0; j < audio_samples_size; j++){
      if (big_index >= BIG_SIZE) break;
      big_audio_samples[big_index] = audio_samples[j];
      big_index++;
    }
  }
 

// https://arm-software.github.io/CMSIS_5/DSP/html/group__ComplexFFT.html
#define  FFT_LEN  256  // this is because  in micro_model_settings.h  : kMaxAudioSampleSize = 512
//int FFT_LEN = BIG_SIZE /2;
// TODO  512/2 = 256 ... but in fact only 480 samples are returned  which is 30 sec of data, TODO: we need to handle 1.2 sec of data
   const static arm_cfft_instance_f32 *S;
   S = & arm_cfft_sR_f32_len256;
   /* instead line above is better to use:
   int status = arm_cfft_init_f32(S, FFT_LEN*2);  // TODO or FFT_LEN   ?
   printf("\n arm_cfft_init_f32 status=%d", status);
   if (status != ARM_MATH_SUCCESS) printf(" ERROR initializing arm_cfft_init_f32");
   */

   // TODO: to feed the audio samples into FFT routine, do we need to convert integer audio array to float array?
   float f_samples[FFT_LEN*2];
   for (int i=0; i < BIG_SIZE; i++){
       f_samples[i] = float(big_audio_samples[i]);
   }

   printf("\n before arm_cfft_f32()");
   arm_cfft_f32(S, f_samples, 0, 1);   // in-place calculation
   //TODO If we can use the calculated samples above
   // we may not need to call  arm_cmplx_mag_f32() below
   printf("\n after arm_cfft_f32()");
   //return;
   // Calculate magnitude of complex numbers output by the FFT.
   // https://arm-software.github.io/CMSIS_5/DSP/html/group__cmplx__mag.html
   float magn[FFT_LEN];
   printf("\n before arm_cmplx_mag_f32()");
   arm_cmplx_mag_f32(f_samples, magn, FFT_LEN);

   printf("\n after arm_cmplx_mag_f32()");
   return;


//  Bernard Berg: find the peaks for the specific frequencies range (~250Hz and ~500Hz):
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
}  //end of audioFFT()

//void test_cmsis_fft(float* data, int data_size, char* name)
void test_cmsis_fft(int* data, int data_size, char* name)
{
  #define FFT_SIZE 2048

  printf("\n name=%s  size=%d", name, data_size);
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
  arm_rfft_q15(&fft_instance, (q15_t*)data, output);

  printf( "\n AFTER arm_rfft_q15 ");

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
  /*
  int i=0;
  while (1) {
     printf("\n %d ", i++);
     if (i > 999999) i=0;
     //audioFFT();
  }
  */

  test_cmsis_fft(i500, 2048 , (char *)(" integer 500Hz "));
  test_cmsis_fft(i250, 2048 , (char *)(" integer 250Hz "));
}
