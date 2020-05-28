#include "mbed.h"
#define ARM_MATH_CM4
#include "arm_math.h"

void setup() {
  // put your setup code here, to run once:
}

void tryInitFFT(const int N) {
  printf("N=%d", N);
  uint8_t ifftFlag = 0; // 0 is FFT, 1 is IFFT
  uint8_t doBitReverse = 1;
  int status;
  
  arm_cfft_radix4_instance_f32 foo1;
  status = arm_cfft_radix4_init_f32(&foo1, N,ifftFlag, doBitReverse);
  printf("status=%d", status);
  
  arm_cfft_radix2_instance_f32 foo2;
  status = arm_cfft_radix2_init_f32(&foo2, N,ifftFlag, doBitReverse);
  printf("status=%d", status);
  
  arm_cfft_radix4_instance_f32 foo3;
  arm_rfft_instance_f32 foo4;
  status = arm_rfft_init_f32(&foo4, &foo3, N, ifftFlag, doBitReverse);
  printf("status=%d", status);

}

void loop() {
  // put your main code here, to run repeatedly:
  int count=0;
  if (count< 1) {
    const int N_N = 8;
    int32_t all_N[] = {16, 32, 64, 128, 256, 512, 1024, 2048};
    for (int i=0; i < N_N; i++) {
      tryInitFFT(all_N[i]);
    }
    //delay(2000);
  }
  
}

int main(void)
{
  loop();
  return 0;
}
