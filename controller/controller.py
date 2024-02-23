import time
import threading
from AlipayTelemetry import *
from LaptopKeyboard import *

myAlipay = Alipay("192.168.86.22", 12346, "ALIPAY")
myIRBeacon = Alipay("192.168.86.27", 12347, "IRBeac1")

# myAlipay = Alipay("192.168.111.171", 12346, "ALIPAY")
# myIRBeacon = Alipay("192.168.111.178", 12347, "IRBeac1")

# myAlipay = Alipay("192.168.175.221", 12346, "ALIPAY")
# myIRBeacon = Alipay("192.168.175.195", 12347, "IRBeac1")

keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
receiver_thread1 = threading.Thread(target=myAlipay.udp_receiver)
receiver_thread2 = threading.Thread(target=myIRBeacon.udp_receiver)

receiver_thread1.start()
receiver_thread2.start()
keyboard_thread.start()  

drivecmd = 0
enabled = "0"
waitForRelease = 0

print(f"IP : {myAlipay.get_laptop_ip()}")

while (True):
    y = 0
    x = 0
    if get_key_state(Key.up):
        y = y + 1
    if get_key_state(Key.down):
        y = y - 1
    if get_key_state(Key.left):
        x = x - 1
    if get_key_state(Key.right): 
        x = x + 1
        
    if x == 0 and y == 0:
        drivecmd = 0
    elif x == 0 and y == 1:
        drivecmd = 1
    elif x == 1 and y == 1:
        drivecmd = 2
    elif x == 1 and y == 0:
        drivecmd = 3
    elif x == 1 and y == -1:
        drivecmd = 4
    elif x == 0 and y == -1:
        drivecmd = 5
    elif x == -1 and y == -1:
        drivecmd = 6
    elif x == -1 and y == 0:
        drivecmd = 7
    elif x == -1 and y == 1:
        drivecmd = 8
            
    if get_key_state(Key.space):     
        if get_key_state(Key.ctrl):
            enabled = 1
            waitForRelease = 1
        else:
            if not waitForRelease:
                enabled = 0
    if not (get_key_state(Key.space)) and not (get_key_state(Key.ctrl)):
        waitForRelease = 0
        
    cmd = f"{drivecmd}{enabled}"
    # print(cmd)
    myIRBeacon.send_udp_packet(f"{enabled}")
    myAlipay.send_udp_packet(cmd)
    time.sleep(0.05)                
                                