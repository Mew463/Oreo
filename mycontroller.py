import socket
import time
import threading
import keyboard


# IP address and port of the Arduino-ESP32
alipay_address = ("192.168.111.171", 12345)    

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

drivecmd = 0
enabled = 0
while (True):
    if keyboard.is_pressed("up"):
        drivecmd = 1
    elif keyboard.is_pressed("right"):
        drivecmd = 2
    elif keyboard.is_pressed("down"):
        drivecmd = 3
    elif keyboard.is_pressed("left"):
        drivecmd = 4
    else:
        drivecmd = 0
    
    if keyboard.is_pressed('space'):
        if keyboard.is_pressed('tab'):
            enabled = 1
        else:
            enabled = 0
    
    cmd = f"{drivecmd}{int(enabled)}"
    send_udp_packet(cmd, alipay_address)
    # print(f"Sent Command: {cmd}")
    time.sleep(0.05)                
                              