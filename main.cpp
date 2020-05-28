/***
For 16KHz and 30ms : 16*3 = 480 samples per second; the reserved buffer size is 512
***/

#include "audio_provider.h"

#define ARM_MATH_CM4
#include "arm_math.h"
#include "arm_const_structs.h"

#include "mbed.h"
void readAudio(int cnt) {
  // printf("\n before GetAudioSamples() cnt=%d", cnt);

  int16_t* audio_samples = nullptr;
  int audio_samples_size = 0;
  int start_ms=0;
  constexpr int kFeatureSliceDurationMs = 30;

  int ret = GetAudioSamples(
                   start_ms,
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);
  //return;
// https://arm-software.github.io/CMSIS_5/DSP/html/group__ComplexFFT.html
#define  FFT_LEN  256   
// TODO  512/2 = 256 ... but in fact only 480 samples are returned ....
   const static arm_cfft_instance_f32 *S;
   // int status = arm_cfft_init_f32(S, FFT_LEN*2);  // TODO or FFT_LEN   ?
   S = & arm_cfft_sR_f32_len256;
   //printf("\n arm_cfft_init_f32 status=%d", status);

   float samples[FFT_LEN*2];
   // populate samples:
   for (int i=0; i < audio_samples_size; i++){
       samples[i] = float(audio_samples[i]);
   }

   arm_cfft_f32(S, samples, 0, 1);

   // Calculate magnitude of complex numbers output by the FFT.
   // https://arm-software.github.io/CMSIS_5/DSP/html/group__cmplx__mag.html
   float magn[FFT_LEN];
   arm_cmplx_mag_f32(samples, magn, FFT_LEN);

//  find peaks in specific frequency range:
    float   mpeak250 = 0.0;
    float   ipeak250 = 0.0;
    float   mpeak500 = 0.0;
    float   ipeak500 = 0.0;
    float freq;
    float m2;

    for (int i=0; i < FFT_LEN; i++){
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
     }
   } 
  
  printf("\n ipeak250=%f mpeak250=%f", ipeak250, mpeak250);
  printf("\n ipeak500=%f mpeak500=%f", ipeak500, mpeak500);
  if (mpeak250 > 9000.0  && mpeak500 > 4000.0){
       printf("\n --- FOUND mpeak250 > 9000.0  && mpeak500 > 4000.0 --- ");
  }
  // printf("\n after GetAudioSamples() ret = %d audio_samples_size=%d", ret, audio_samples_size);
}

int main(void)
{
  // KF66: 16KHz *30 ms =  480 samples per sec,i the output array size:  512
  int i=0;
  while (1) {
     printf("\n %d ", i++);
     if (i > 999999) i=0;
     readAudio(1);
    
  }
  return 0;
}
