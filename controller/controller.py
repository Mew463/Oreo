# import time
import threading
from LaptopKeyboard import *
from bluetooth import *
import asyncio

ir_beacon = BLE_UART(peripheral_name='IR Beacon')
alipay = BLE_UART(peripheral_name='Alipay')


keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
keyboard_thread.daemon = True
keyboard_thread.start()  

cmd = ""

async def ir_beacon_receive():
    while True:
        await asyncio.sleep(0.1)
        if (ir_beacon.isConnected):
            print(f"[IR BEACON] {await ir_beacon.read()}")
async def alipay_receive():
    while True:
        await asyncio.sleep(0.1)
        if (alipay.isConnected):
            print(f"[ALIPAY] {await alipay.read()}")
    
async def alipay_comm():
    await alipay.connect()
    while True:
        await asyncio.sleep(0.05)
        if (alipay.isConnected):
            await alipay.write(cmd)
        else:  
            await alipay.connect()
            
async def ir_beacon_comm():
    await ir_beacon.connect()
    while True:
        await asyncio.sleep(0.05)
        if (ir_beacon.isConnected):
            await ir_beacon.write(cmd)
        else:
            await ir_beacon.connect()
            
async def cmd_handler():
    global cmd
    enabled = 0
    waitForRelease = 0
    while True:
        x,y,drivecmd,tuning = (0,)*4
        
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
            
        if get_key_state('u'):
            tuning = 1
        elif get_key_state('j'):
            tuning = 2
        elif get_key_state('i'):
            tuning = 3
        elif get_key_state('k'):
            tuning = 4
        elif get_key_state('o'):
            tuning = 5
        elif get_key_state('l'):
            tuning = 6
            
        if get_key_state(Key.space):     
            if get_key_state(Key.ctrl):
                enabled = 1
                waitForRelease = 1
            else:
                if not waitForRelease:
                    enabled = 0
        if not (get_key_state(Key.space)) and not (get_key_state(Key.ctrl)):
            waitForRelease = 0
            
        cmd = f"{enabled}{drivecmd}{tuning}000"
        # print(cmd)
        await asyncio.sleep(0.05)

async def main():
    await asyncio.gather(cmd_handler(), ir_beacon_comm(), alipay_comm(), ir_beacon_receive(), alipay_receive())

asyncio.run(main())

    
       
                                