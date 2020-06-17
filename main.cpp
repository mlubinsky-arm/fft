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

DigitalOut led(LED1);

#include "2048_log_coeff.h"
// Files for testing CMSIS-FFT:

//#include "i500Hz_2048points.h"
//#include "i250Hz_2048points.h"
//#include "iSound.h"
#define COLLECTION 0
#define PREDICTION 1
#define FFT_SIZE 2048
//#define FFT_SIZE 1024
void cmsis_fft(int* data, int data_size);

void audioFFT() {
    static int32_t  previous_timestamp=0;
    static int  fft_samples[FFT_SIZE];
    int big_index=0;
    for (int i=0; i < FFT_SIZE; i++){
        fft_samples[i]=0;
    }

    //int start_ms=0;
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
                   k * kFeatureSliceDurationMs, // TODO
                   kFeatureSliceDurationMs,
                   &audio_samples_size,
                   &audio_samples);

      for ( int j=0; j < audio_samples_size; j++){
        if (big_index >= FFT_SIZE) break;
        fft_samples[big_index] = int (audio_samples[j]);
        big_index++;
      }
      /*
      int32_t  latest_timestamp = LatestAudioTimestamp();
      while (latest_timestamp - previous_timestamp < kFeatureSliceDurationMs){
        ThisThread::sleep_for(kFeatureSliceDurationMs);
        latest_timestamp = LatestAudioTimestamp();
      }
      previous_timestamp = latest_timestamp;
      */
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

  //int mode=COLLECTION;
  int mode=PREDICTION;
 
  if ( mode == PREDICTION) {
/*    
int OLD_MODEL=1;     
if (OLD_MODEL) {    
   #define MODEL_SIZE 10
   float coeff[MODEL_SIZE] = {
     0.10956007, -0.13896823,  0.01236551,  0.23775266,  0.55015138,
     0.03702403,  0.01534263,  -0.06332641, -0.08341657, -0.07537096
   };

   int fft_index[MODEL_SIZE]={242, 246, 250, 254, 258, 492, 496, 500, 504, 508};

   float linear = -1.45595351; // model intercept
   for (int i=0; i<MODEL_SIZE; i++){
     linear = linear +  (coeff[i] * s[fft_index[i]]);
   }
}
else {  // NEW MODEL
*/
  float linear = intercept;
   for (int i=0; i<2048; i++){
     linear = linear +  (coeff[i] * s[i]);
   }
  //} // NEW MODEL   
   //Logistic regression
   float  lr = 1.0 / (1.0 + exp(-linear));
   printf ("\n logistic_regression=%f   linear=%f  s[0]=%d", lr, linear, s[0]);
   led=1; //off

   if (lr > 0.5) {
        printf ("\n ----------------Blink-----------------\n");
        led = 0; // on
        ThisThread::sleep_for(800); //ms
        led=1; // off
        /*
         for (int i=0; i<2048; i++){
           printf("%d ",s[i]);
           if (i%32 == 0) printf("\n");
         }
         printf("\n");
         */
   }
  }  // END OF PREDICTON


  if (mode == COLLECTION){

   static int header=0;
/*   
    static char *fmt_float=
// 1    2    3    4    5  6   7    8    9   10    11   12  13   14   15   16   17    18   19   20   21   22   23   24   25   26   27   28   29  30    31   32
"%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\
%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
//   ;
*/
  static const char *fmt_int=
  //1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
  "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
  ;

  if (header == 0){
      float freq;
      for (int i=0; i < data_size; i++) {  // 2048 times this is slow
          freq =  float(i*16000.0)/FFT_SIZE;
          printf("%.2f,",freq);
      }

      printf("\n");
      header=1; // to call it once
  }
  // To speedup we call printf() once per 64 datapoins; another possible improvement: use sprintf
  for (int i=0; i < data_size-1; i+=64) {

    printf(fmt_int,s[i],s[i+1],s[i+2],s[i+3],s[i+4],s[i+5],s[i+6],s[i+7],s[i+8],s[i+9],s[i+10],s[i+11],s[i+12],s[i+13],s[i+14],s[i+15],
                   s[i+16],s[i+17],s[i+18],s[i+19],s[i+20],s[i+21],s[i+22],s[i+23],s[i+24],s[i+25],s[i+26],s[i+27],s[i+28],s[i+29],s[i+30],s[i+31],
                   s[i+32],s[i+33],s[i+34],s[i+35],s[i+36],s[i+37],s[i+38],s[i+39],s[i+40],s[i+41],s[i+42],s[i+43],s[i+44],s[i+45],s[i+46],s[i+47],
                   s[i+48],s[i+49],s[i+50],s[i+51],s[i+52],s[i+53],s[i+54],s[i+55],s[i+56],s[i+57],s[i+58],s[i+59],s[i+60],s[i+61],s[i+62],s[i+63]
          );

  }
  /*    slow but works
   for (int i=0; i < data_size; i++) {
    printf("%d , ", s[i]);
   }
  */
  printf("\n");

  }   // end of if (mode == COLLECTION){
}  // end of cmsis_fft

int main(void)
{
  // Test CMSIS FFT:
  //cmsis_fft(i500,  FFT_SIZE );
  //cmsis_fft(i250,  FFT_SIZE );
  //cmsis_fft(isound, FFT_SIZE );

  while (1) {
     audioFFT();
  }
}
