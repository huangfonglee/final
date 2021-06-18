import time, serial


serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)


while(True): 
    line = s.read(25)
    print(line.decode())
    time.sleep(1)
