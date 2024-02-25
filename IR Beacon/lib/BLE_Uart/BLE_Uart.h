#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#include <NimBLEDevice.h>
#include <USB.h>

class BLE_Uart {
    public:
        BLE_Uart(char* _packetBuffer, int _packSize);
        void init_ble();
        void send(char* _msg);
        bool isConnected();
    private:
        
};