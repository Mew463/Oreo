# `sudo pkill bluetoothd` if bluetooth not working on ming's mac m3 pro

import threading
from LaptopKeyboard import *
from bluetooth import *
import asyncio

ir_beacon_2 = BLE_UART(peripheral_name='IR Beacon 2', address = 'CBB4A195-F34C-E6C5-21CD-ACBB7D44352A')
ir_beacon_1 = BLE_UART(peripheral_name='IR Beacon 1', address = 'CC80B9F1-FE04-64E9-2CE8-014A24EEE1BF')
alipay = BLE_UART(peripheral_name='Alipay', address = 'BCF4B026-C413-FC75-D78E-DE567078A888')


keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
keyboard_thread.daemon = True
keyboard_thread.start()  

cmd = ""

async def bluetooth_receive_handler(BLE_DEVICE):
    while True:
        await asyncio.sleep(0.1)
        if (BLE_DEVICE.isConnected):
            print(f"[{BLE_DEVICE._peripheral_name}] {await BLE_DEVICE.read()}")

async def bluetooth_comm_handler(BLE_DEVICE):
    await BLE_DEVICE.connect()
    while True:
        await asyncio.sleep(0.05)
        if (BLE_DEVICE.isConnected):
            await BLE_DEVICE.write(cmd)
        else:  
            await BLE_DEVICE.connect()
            
async def cmd_handler():
    global cmd
    enabled = 0
    waitForEnableReleased = 0
    lastState = 0
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
        elif get_key_state('t'):
            tuning = 7
        elif get_key_state('g'):
            tuning = 8
            
        if get_key_state(Key.space):     
            if get_key_state(Key.ctrl):
                enabled = 1
                waitForEnableReleased = 1
            else:
                if not waitForEnableReleased:
                    enabled = 0
        if not (get_key_state(Key.space)) and not (get_key_state(Key.ctrl)):
            waitForEnableReleased = 0
        
        curState = get_key_state('0')
        if curState and not lastState:
            if enabled == 1:
                enabled = 2
            elif enabled == 2:
                enabled = 1
        lastState = curState
            
        cmd = f"{enabled}{drivecmd}{tuning}000"
        # print(cmd)
        await asyncio.sleep(0.05)

async def main():
    await asyncio.gather(cmd_handler(), bluetooth_comm_handler(ir_beacon_1), bluetooth_comm_handler(alipay), bluetooth_receive_handler(ir_beacon_1), bluetooth_receive_handler(alipay), bluetooth_comm_handler(ir_beacon_2), bluetooth_receive_handler(ir_beacon_2))

asyncio.run(main())

    
       
                                