## FFT audio example
```
mbed compile --target K66F --toolchain GCC_ARM --profile release --flash --sterm --baudrate  115200
```

## Reading data from the serial port using screen

File ```screen.sh``` allows to read from the serial port using screen.
Output will be in file named screenlog.0

To exit the screen hit "Ctrl-A" followed by "k"

## Readingi data from the serial port using minicom

File ```min.sh``` allows to read from the serial port using minicom.
Output liww be in file log.txt

Install minicom  (brew install minicom).

On Mac configure Terminal.app: Preferncess -> Profiles -> Keyboard -> Checkbox "Use Options as Meta key"

To exit from minicom under Mac Terminal: Press "Options and x".

## CMSIS FFT

<http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.447.4272&rep=rep1&type=pdf>

From the link above:
```
 CMSIS algorithm can compute complex FFT of the arrays of size 16, 64, 256 or 1024. 
For the real FFT it can be 128, 512 or 2048 samples. 
```


<https://arm-software.github.io/CMSIS_5/DSP/html/group__RealFFT.html>

<https://arm-software.github.io/CMSIS_5/DSP/html/group__ComplexFFT.html>

<https://arm-software.github.io/CMSIS_5/DSP/html/arm__fft__bin__example__f32_8c.html>

<https://www.nxp.com/docs/en/application-note/AN12383.pdf>

<https://ios.developreference.com/article/17946093/proper+FFT+length+for+ARM+CMSIS+DSP+fft+function>

<https://ez.analog.com/analog-microcontrollers/ultra-low-power-microcontrollers/f/q-a/19451/cmsis-dsp-fft-functionality-increases-size-of-the-project-drastically>

<http://m0agx.eu/2018/05/23/practical-fft-on-microcontrollers-using-cmsis-dsp/>

