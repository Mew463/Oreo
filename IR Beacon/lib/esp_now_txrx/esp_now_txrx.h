#include <esp_now.h>
#include <WiFi.h>

class ESP_NOW_TXRX {
    public:
        ESP_NOW_TXRX(uint8_t *_receiver_address, int _packetSize);
        void init(esp_now_recv_cb_t receiv_cb);
        void getMacAddress();
        void send(const uint8_t *bytes);

    private:
        int packetSize;
        uint8_t *receiver_address;
};