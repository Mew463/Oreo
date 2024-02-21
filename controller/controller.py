import socket
import time
import threading
from pynput.keyboard import Key, Listener, KeyCode

key_state = {}

# Function to be called when a key is pressed
def on_press(key):
    key_state[key] = True  # Update key state to pressed

# Function to be called when a key is released
def on_release(key):
    key_state[key] = False  # Update key state to released
    
def get_key_state(key):
    if type(key) == str:
        key = KeyCode.from_char(key)
    if (key in key_state and key_state[key]):
        return True
    else:
        return False
    
# Start the keyboard listener in a separate thread
keyboard_thread = threading.Thread(target=lambda: Listener(on_press=on_press, on_release=on_release).start())
keyboard_thread.start()

# IP address and port of the Arduino-ESP32
# alipay_address = ("192.168.86.22", 12345)    
alipay_address = ("192.168.111.177", 12345)    

def udp_receiver():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
    sock.bind(("0.0.0.0", 12346))
    
    while True:
        data, addr = sock.recvfrom(64)
        try:
            data_str = data.decode("utf-8")
            print(f"[ALIPAY] {data_str}")
        except:
            pass

def send_udp_packet(data, address):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 128)
        sock.sendto(data.encode(), address)

receiver_thread = threading.Thread(target=udp_receiver)
receiver_thread.daemon = True  # This allows the thread to exit when the main thread exits
receiver_thread.start()


print(f"IP Address: {socket.gethostbyname_ex(socket.gethostname())[-1][1]}")


drivecmd = 0
enabled = 0

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
        if get_key_state(Key.tab):
            enabled = 1
        else:
            enabled = 0
    
    cmd = f"{drivecmd}{int(enabled)}"
    # send_udp_packet(cmd, alipay_address)
    # print(f"Sent Command: {cmd}")
    time.sleep(0.05)                
                              