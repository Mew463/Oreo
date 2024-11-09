# `sudo pkill bluetoothd` if bluetooth not working on ming's mac m3 pro
import threading
from LaptopKeyboard import *
from bluetooth import *
import asyncio
import time
import streamlit as st

startTime = time.time()

def millis():
    return round((time.time()-startTime) * 1000)

ir_beacon_2 = BLE_UART(peripheral_name='Beac 2', address = '9AC0FFF3-446C-1A64-DA89-1376064B2BA1')
ir_beacon_1 = BLE_UART(peripheral_name='Beac 1', address = 'DBA047A7-4143-45D5-E469-FEEA2E354502')
oreo = BLE_UART(peripheral_name='Oreo', address = '1932D032-A476-F238-07F0-A39D5208BC73')
bt_devices = {ir_beacon_2, ir_beacon_1, oreo}
# hockey_puck = BLE_UART(peripheral_name='Hockey Puck', address = '07C80925-7C0F-1236-FB54-CFC3912A3B9D') 

keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
keyboard_thread.daemon = True
keyboard_thread.start()  

robotcmd = ""
irbeaconcmd = ""
enabled = 0
activeBeacon = 1
lastBeaconRead = millis()
calibrate_accel = 0

async def bluetooth_receive_handler(BLE_DEVICE):
    global lastBeaconRead
    while True:
        await asyncio.sleep(0.1)
        if (BLE_DEVICE.isConnected):
            msg = await BLE_DEVICE.read()
            if (msg == "seen"):
                if (lastBeaconRead < millis()): # Some reason it thinks its "seen" right after enabling
                    lastBeaconRead = millis()
            elif (msg is not None):
                print(f"[{BLE_DEVICE._peripheral_name}] {msg}")

async def bluetooth_comm_handler(BLE_DEVICE, isMainRobot):
    await BLE_DEVICE.connect()
    while True:
        await asyncio.sleep(0.05)
        if (BLE_DEVICE.isConnected):
            if (isMainRobot):
                await BLE_DEVICE.write(robotcmd)
            else:
                await BLE_DEVICE.write(irbeaconcmd)
        else:  
            await BLE_DEVICE.connect()

              
async def ir_beacon_switcher(): 
    global enabled
    global lastBeaconRead  
    while True:
        await asyncio.sleep(0.1)
        if (enabled != 1):
            lastBeaconRead = millis() + 2000 # Add some time so the beacon doesnt switch right after enabling melty brain mode
        if (enabled == 1 and millis() - lastBeaconRead > 1000 and ir_beacon_2.isConnected and ir_beacon_1.isConnected):
            toggleBeacon()
            
def toggleBeacon():
    global activeBeacon
    global lastBeaconRead
    lastBeaconRead = millis()
    if activeBeacon == 1:
        activeBeacon = 2
    elif activeBeacon == 2:
        activeBeacon = 1
            
async def cmd_handler():
    global robotcmd
    global irbeaconcmd
    global enabled
    global activeBeacon
    waitForEnableReleased = 0
    lastState = 0
    drivestate = 1
    while True:
        x,y,drivecmd,robottuning,irbeacontuning,boost = (0,)*6  
        
        if get_key_state("Key.up"): 
            y = y + 1
        if get_key_state("Key.down"):
            y = y - 1
        if get_key_state("Key.left"):
            x = x - 1
        if get_key_state("Key.right"): 
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
            
        if get_key_state('q'):
            robottuning = 1
        elif get_key_state('a'):
            robottuning = 2
        elif get_key_state('w'):
            robottuning = 3
        elif get_key_state('s'):
            robottuning = 4
        elif get_key_state('e'):
            robottuning = 5
        elif get_key_state('d'):
            robottuning = 6
        
        if get_key_state('t'):
            irbeacontuning = 1
        elif get_key_state('g'):
            irbeacontuning = 2
            
        if get_key_state("Key.space"):     
            if get_key_state("Key.ctrl"):
                enabled = drivestate
                waitForEnableReleased = 1
            else:
                if not waitForEnableReleased:
                    enabled = 0
        if not (get_key_state("Key.space")) and not (get_key_state("Key.ctrl")):
            waitForEnableReleased = 0
        
        if (enabled != 0):
            if (get_key_state('z') or get_key_state('Z')):
                enabled = drivestate = 1
            if (get_key_state('x') or get_key_state('X')):
                enabled = drivestate = 2
        
        curState = get_key_state('1')
        if curState and not lastState:
            toggleBeacon()
        lastState = curState
        
        if (get_key_state("Key.shift")):
            boost = 1 
        
        if get_key_state("u"):
            calibrate_accel = 1
        elif get_key_state("j"):
            calibrate_accel = 2
        else:
            calibrate_accel = 0

        robotcmd = f"{enabled}{drivecmd}{robottuning}{boost}{calibrate_accel}0"
        irbeaconcmd = f"{enabled}{activeBeacon}{irbeacontuning}000"
        await asyncio.sleep(0.05)
        
async def main_async_tasks():
    await asyncio.gather(ir_beacon_switcher(), cmd_handler(), bluetooth_comm_handler(ir_beacon_1, False), bluetooth_comm_handler(oreo, True), bluetooth_receive_handler(ir_beacon_1), bluetooth_receive_handler(oreo), bluetooth_comm_handler(ir_beacon_2, False), bluetooth_receive_handler(ir_beacon_2))
    # await asyncio.gather(ir_beacon_switcher(), cmd_handler(), bluetooth_comm_handler(ir_beacon_1, False), bluetooth_comm_handler(oreo, True), bluetooth_comm_handler(hockey_puck, True), bluetooth_receive_handler(hockey_puck), bluetooth_receive_handler(ir_beacon_1), bluetooth_receive_handler(oreo), bluetooth_comm_handler(ir_beacon_2, False), bluetooth_receive_handler(ir_beacon_2))

def start_async_tasks():
    asyncio.run(main_async_tasks())


if "Running" not in st.session_state:
    st.session_state["Running"] = True
    async_thread = threading.Thread(target=start_async_tasks)
    async_thread.start()

# Display UI in Streamlit, updating based on session state
st.title("Oreo Control Panel")
st.header("Device Status")

# Use an empty container to update just the status UI
status_container = st.empty()

# Update only the device status UI
# while True:
with status_container.container():
    status_cols = st.columns(len(bt_devices))
    for i, bt_device in enumerate(bt_devices):
        with status_cols[i]:
            st.subheader(bt_device._peripheral_name)
            status = bt_device.isConnected
            if status:
                st.success("Connected")
            else:
                st.error("Disconnected")
time.sleep(2)  # Update interval in seconds
st.rerun()