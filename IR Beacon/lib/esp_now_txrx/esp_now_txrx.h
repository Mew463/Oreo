#include <esp_now.h>
#include <WiFi.h>

class ESP_NOW_TXRX {
    public:
        ESP_NOW_TXRX(uint8_t *_receiver_address, int _packetSize);
        void init(esp_now_recv_cb_t receiv_cb);
        void getMacAddress();
        void send(const uint8_t *bytes);

    private:
        // uint8_t* packetBuffer;
        int packetSize;
        uint8_t *receiver_address;
        // void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
};