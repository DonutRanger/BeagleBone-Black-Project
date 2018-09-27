#Establish usb_modeswitch to sierra wireless VID:PID
sudo usb_modeswitch -v 1199 -p 68c0 -S 1 -W

#Remove present usbserial
sudo modprobe -r usbserial

#Insert usbserial module
sudo modprobe usbserial vendor=0x1199 product=0x68c0

cd /dev
ls -l ttyU*

