/***
For 16KHz and 30ms : 16*3 = 480 samples per second; the reserved buffer is 512
***/
#include "audio_provider.h"

#define ARM_MATH_CM4
#include "arm_math.h"

#include "mbed.h"
void readAudio(int cnt) {
  printf("\n before GetAudioSamples() cnt=%d", cnt);

  int16_t* audio_samples = nullptr;
  int audio_samples_size = 0;
  int start_ms=0;
  constexpr int kFeatureSliceDurationMs = 30;

  int ret = GetAudioSamples(
                   start_ms,
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);

  printf("\n after GetAudioSamples() ret = %d audio_samples_size=%d", ret, audio_samples_size);
}

void try_init_FFT(int N) {
  printf("\n try_init_FFT():  N=%d", N);
  uint8_t ifftFlag = 0; // 0 is FFT, 1 is IFFT
  uint8_t doBitReverse = 1;
  int status;
  
  arm_cfft_radix4_instance_f32 foo1;
  status = arm_cfft_radix4_init_f32(&foo1, N,ifftFlag, doBitReverse);
  printf("\n  arm_cfft_radix4_init_f32  status=%d", status);
  
  arm_cfft_radix2_instance_f32 foo2;
  status = arm_cfft_radix2_init_f32(&foo2, N,ifftFlag, doBitReverse);
  printf("\n  arm_cfft_radix2_init_f32 status=%d", status);
  
  arm_cfft_radix4_instance_f32 foo3;
  arm_rfft_instance_f32 foo4;
  status = arm_rfft_init_f32(&foo4, &foo3, N, ifftFlag, doBitReverse);
  printf("\n  arm_rfft_init_f32 status=%d \n", status);

}

void test_init_FFT() {
    printf("\n START test_init_FFT()");

    int32_t n2=2;
    for (int i=1; i < 11; i++) {
     // printf("\n i=%d", i);
      n2 = n2 *2;
      try_init_FFT(n2);
    }
    printf("\n END test_init_FFT()");
  
}

int main(void)
{
  readAudio(1);  // KF66: 16KHz *30 ms =  480 samples per sec,i the output array size:  512
  test_init_FFT();
  return 0;
}
