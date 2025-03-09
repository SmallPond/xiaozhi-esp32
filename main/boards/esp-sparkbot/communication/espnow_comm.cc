#include "espnow_comm.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <string.h>

#define TAG "ESP_NOW"

namespace iot {


ESPNowComm::ESPNowComm(const uint8_t* address) {
    // Default to broadcast address if no address is provided
    if (address == nullptr) {
        memcpy(send_address_, defaultsend_address_, 6);
    } else {
        memcpy(send_address_, address, 6);
    }
    is_inited_ = false;
}

ESPNowComm::~ESPNowComm() {
    esp_now_deinit();
}

int ESPNowComm::Init() {
    esp_err_t ret = ESP_OK;
    // esp_err_t ret = esp_netif_init();
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to initialize netif: %s", esp_err_to_name(ret));
    //     return -1;
    // }

    // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    // ret = esp_wifi_init(&cfg);
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
    //     return -1;
    // }

    // ret = esp_wifi_set_mode(WIFI_MODE_STA);
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
    //     return -1;
    // }

    // ret = esp_wifi_start();
    // if (ret != ESP_OK) {
    //     ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
    //     return -1;
    // }

    if (is_inited_) {
        return 0;
    }
    // Initialize ESP-NOW
    ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ESP-NOW: %s", 
                            esp_err_to_name(ret));
        return -1;
    }

    // Add peer device
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, send_address_, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    ret = esp_now_add_peer(&peerInfo);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add peer: %s", esp_err_to_name(ret));
        return -1;
    }

    wifi_ap_record_t wifi_info;
    ret = esp_wifi_sta_get_ap_info(&wifi_info);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Current WiFi Channel: %d\n", wifi_info.primary);
    } else {
        ESP_LOGE(TAG, "Failed to get WiFi info: %s\n", esp_err_to_name(ret));
    }

    is_inited_ = true;


    return 0;
}

int ESPNowComm::Send(const std::string& str) {
    if (!Init()) {
        ESP_LOGE(TAG, "ESP Now init failed before sending msg.");
        return -1;
    }
    // Send data to the specified address
    esp_err_t result = esp_now_send(send_address_, (uint8_t*)str.c_str(), str.size());
    return (result == ESP_OK) ? 0 : -1;
}

void ESPNowComm::SetRecvCallback(RecvCallback callback) {
    // Temporarily not implemented
}

} // namespace iot