import time
import threading
from AlipayTelemetry import *
from LaptopKeyboard import *

myAlipay = Alipay("192.168.86.22", 12346, "ALIPAY")
myIRBeacon = Alipay("192.168.86.27", 12347, "IRBeac1")

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
    if get_key_state(Key.up):
        drivecmd = 1
    elif get_key_state(Key.left):
        drivecmd = 2
    elif get_key_state(Key.down):
        drivecmd = 3
    elif get_key_state(Key.right):
        drivecmd = 4
    else:
        drivecmd = 0
    
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
    myAlipay.send_udp_packet(cmd)
    myIRBeacon.send_udp_packet(f"{enabled}")
    time.sleep(0.05)                
                              