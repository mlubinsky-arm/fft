# Looking for the serial port:
#-------------------------------
m=`ls /dev/tty.usbmodem*`
status=$?
echo $status
if test $status -ne 0 
then
  echo  "Not found the board under /dev/tty.usbmodem*"
  exit
fi

echo $m

# Run screen command
# ----------------------
# To exit the screen hit "Ctrl-A" followed by "k"
# Output will be in file named screenlog.0
sudo screen -L $m  115200

