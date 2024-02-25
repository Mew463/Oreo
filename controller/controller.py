import time
import threading
from LaptopKeyboard import *
from bluetooth import *

ir_beacon = BLE_UART(peripheral_name='IR Beacon')
alipay = BLE_UART(peripheral_name='Alipay')


keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
keyboard_thread.daemon = True
keyboard_thread.start()  

async def main():
    await ir_beacon.connect()
    await alipay.connect()
    drivecmd = 0
    enabled = 0
    waitForRelease = 0
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
            
        cmd = f"{enabled}{drivecmd}0000"
        if (ir_beacon.isConnected):
            await ir_beacon.write(cmd)          
        if (alipay.isConnected):
            await alipay.write(cmd)

asyncio.run(main())

    
       
                                