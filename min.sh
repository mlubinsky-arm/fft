# Reading from the serial port using minicom
# Install minicom  (brew install minicom)
# On Mac configure Terminal.app: Preferncess -> Profiles -> Keyboard -> Checkbox "Use Options as Meta key"
# To exit from minicom under Mac Terminal: Press "Options and x"

m=`ls /dev/tty.usbmodem*`
status=$?
echo $status
if test $status -ne 0 
then
  echo  "Not found the board under /dev/tty.usbmodem*"
  exit
fi

echo $m
#  Output file : log.txt 
R=115200
minicom -b $R -D $m -C log.txt


