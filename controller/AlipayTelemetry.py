import socket

class Alipay:
    def __init__(self, targetIP):
        self.targetIP = targetIP
        self.receiveBufferSize = 128
        
    def get_laptop_ip(self):
        laptop_ip = ""
        while (laptop_ip == ""):
            laptop_ip = socket.gethostbyname_ex(socket.gethostname())[-1][1]
        return laptop_ip
    
    def udp_receiver(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
        sock.bind(("0.0.0.0", 12346))
        
        while True:
            data, addr = sock.recvfrom(64) # This code is blocking!! Implement whether connected / disconnected from Alipay??
            try:
                data_str = data.decode("utf-8")
                print(f"[ALIPAY] {data_str}")
            except:
                pass
            
    def send_udp_packet(self, data):
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, self.receiveBufferSize)
            sock.sendto(data.encode(), (self.targetIP, 12345))