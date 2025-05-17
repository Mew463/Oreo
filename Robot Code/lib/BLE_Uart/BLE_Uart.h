#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#include <NimBLEDevice.h>
#include <unordered_map>
class BLE_Uart {
    public:
        BLE_Uart(char* _packetBuffer, int _packSize);
        void init_ble(const std::string &name);
        void send(String message);
        void send(float value);
        bool isConnected();
    private:
};

enum class ROBOT_MODES {
    IDLE,
    MELTY,
    TANK
};

static std::unordered_map<char, ROBOT_MODES> RobotModeMap{
    {'0', ROBOT_MODES::IDLE},
    {'1', ROBOT_MODES::MELTY},
    {'2', ROBOT_MODES::TANK}
};

class Robot_BLE_Uart : public BLE_Uart {
    public:
        Robot_BLE_Uart(char* _packetBuffer, int _packSize) : BLE_Uart(_packetBuffer, _packSize) {};
        ROBOT_MODES getMode();
};