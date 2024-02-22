import time
import threading
from AlipayTelemetry import *
from LaptopKeyboard import *

myAlipay = Alipay("192.168.111.177")
    
keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
receiver_thread = threading.Thread(target=myAlipay.udp_receiver)
receiver_thread.start()
keyboard_thread.start()  

drivecmd = 0
enabled = "0"
waitForRelease = 0

print(f"IP : {myAlipay.get_laptop_ip()}")

while (True):
    if get_key_state(Key.up):
        drivecmd = 1
    elif get_key_state(Key.right):
        drivecmd = 2
    elif get_key_state(Key.down):
        drivecmd = 3
    elif get_key_state(Key.left):
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
    # print(f"Command: {cmd}")
    time.sleep(0.05)                
                              