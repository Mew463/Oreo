import socket
import time
import keyboard

def send_udp_packet(data, address):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 8192)  # Buffer size 8192
        sock.sendto(data.encode(), address)

# IP address and port of the Arduino-ESP32
arduino_address = ("192.168.43.71", 12345)

last_throttle_command = "n"  # Default to "n" for neutral
steering_value = 0  # Default steering value
power_value = 7  # Default power value (integer from 0 to 100)
radius_value = 3.21  # Default radius value
enabled = False  # Default to enabled
rpm = 600

print_counter = 0
print_interval = 10  # Print every 10 loops

while True:
    # Check for throttle command (up for forward, down for backward)
    if keyboard.is_pressed("up"):
        last_throttle_command = "f"
    elif keyboard.is_pressed("down"):
        last_throttle_command = "b"
    else:
        last_throttle_command = "n"

    # Check for steering command (left and right arrow keys)
    if keyboard.is_pressed("r"):
        steering_value -= 0.1  # No limit on steering_value adjustment
    elif keyboard.is_pressed("l"):
        steering_value += 0.1

    # Check for power command (u for increase, d for decrease)
    if keyboard.is_pressed("u"):
        power_value = min(100, power_value + 1)
    elif keyboard.is_pressed("d"):
        power_value = max(0, power_value - 1)

    # Adjust radius using f and c keys
    if keyboard.is_pressed("f"):
        radius_value += 0.01
    elif keyboard.is_pressed("c"):
        radius_value = max(0, radius_value - 0.01)

    # Toggle enabled field (e for enable, s for disable)
    if keyboard.is_pressed("e"):
        enabled = 'true'
    elif keyboard.is_pressed("s"):
        enabled = 'false'

    power_boost = 0
    if keyboard.is_pressed("p"):
        power_boost = 25
    # Adjust rpm
    if keyboard.is_pressed("3"):
        rpm += 100
    elif keyboard.is_pressed("1"):
        rpm = max(0, rpm - 100)

    # Construct the packet string with throttle, steering, power, radius, enabled, and timestamp
    timestamp = int(time.time() * 100 - 170633619600)
    s = steering_value
    # Check for steering command (left and right arrow keys)
    steering_power = 0.5
    if keyboard.is_pressed("q"):
        steering_power = 1
    if keyboard.is_pressed("left"):
        s += steering_power  # No limit on steering_value adjustment
    elif keyboard.is_pressed("right"):
        s -= steering_power
    power_command = max(power_boost, power_value)
    packet = f"rpm={rpm}&.throttle={last_throttle_command}&steering={s:.2f}&power={power_command}&radius={radius_value:.2f}&timestamp={timestamp}&enabled={enabled}&"

    # Send the packet to the Arduino-ESP32
    send_udp_packet(packet, arduino_address)
    print("Sent Command:", packet)

    # Increment the print counter
    print_counter += 1

    # Print the sent command every 10 loops
    if print_counter == print_interval:
        # print("Sent Command:", packet)
        print_counter = 0  # Reset the counter

    # Wait for a short interval to avoid sending too many packets
    time.sleep(0.1)