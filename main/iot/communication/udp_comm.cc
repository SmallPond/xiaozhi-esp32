#include "udp_comm.h"

#define TAG "UDPComm"

namespace iot {

UDPComm::UDPComm(const std::string& target_ip, int target_port)
    : sock_(-1),
      target_ip_(target_ip),
      bind_port_(DEFAULT_UDP_PORT),
      target_port_(target_port),
      recv_callback_(nullptr) {
}

UDPComm::~UDPComm() {
    if (sock_ != -1) {
        close(sock_);
    }
}

int UDPComm::Init() {

    if (is_inited) {
        return 0;
    }
    // 初始化 bind_ip_ 和 target_ip_
    bind_ip_ = GetLocalIpAddress();
    if (bind_ip_.empty()) {
        ESP_LOGE(TAG, "Failed to get local IP address");
        return -1;
    }

    if (target_ip_.empty()) {
        target_ip_ = GetBroadcastAddress(bind_ip_);
        if (target_ip_.empty()) {
            ESP_LOGE(TAG, "Failed to get broadcast address");
            return -1;
        }
    }

    ESP_LOGI(TAG, "get local IP %s, target IP %s", bind_ip_.c_str(), 
                            target_ip_.c_str());

    // 创建 socket
    sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock_ < 0) {
        ESP_LOGE(TAG, "Failed to create socket");
        return -1;
    }

    // 绑定 socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(bind_port_);
    inet_pton(AF_INET, bind_ip_.c_str(), &server_addr.sin_addr);

    if (bind(sock_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind socket");
        close(sock_);
        sock_ = -1;
        return -1;
    }

    // 启用广播
    int broadcast_enable = 1;
    if (setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, 
                        sizeof(broadcast_enable)) < 0) {
        ESP_LOGE(TAG, "Failed to enable broadcast");
        close(sock_);
        sock_ = -1;
        return -1;
    }

    // 启动接收任务
    // xTaskCreate(UDPComm::RecvTask, "udp_recv_task", 4096, this, 5, nullptr);
    is_inited = true;
    return 0;
}

int UDPComm::Send(const std::string& str) {

    if (Init()) {
        ESP_LOGE(TAG, "init failed.");
        return -1;
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(target_port_);;
    inet_pton(AF_INET, target_ip_.c_str(), &dest_addr.sin_addr);
    ESP_LOGI(TAG, "Send to %s:%d", target_ip_.c_str(), target_port_);
    int err = sendto(sock_, str.c_str(), str.length(), 0, (struct sockaddr*)&dest_addr,
                                 sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Failed to send data");
        return -1;
    }

    return 0;
}

void UDPComm::SetRecvCallback(RecvCallback callback) {
    recv_callback_ = callback;
}
/*
void UDPComm::RecvTask(void* arg) {
    UDPComm* udp_comm = static_cast<UDPComm*>(arg);
    char buffer[1024];
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    while (true) {
        int len = recvfrom(udp_comm->sock_, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&source_addr, &addr_len);
        if (len > 0) {
            buffer[len] = '\0';
            if (udp_comm->recv_callback_) {
                std::vector<uint8_t> data(buffer, buffer + len);
                udp_comm->recv_callback_(data);
            }
        } else {
            ESP_LOGE(TAG, "Failed to receive data");
        }
    }
}
*/

std::string UDPComm::GetLocalIpAddress() {
    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = esp_netif_get_default_netif();
    if (netif == nullptr || esp_netif_get_ip_info(netif, &ip_info) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get local IP address");
        return "";
    }
    char ip_str[16];
    inet_ntoa_r(ip_info.ip.addr, ip_str, sizeof(ip_str));
    return std::string(ip_str);
}

std::string UDPComm::GetBroadcastAddress(const std::string& local_ip) {
    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = esp_netif_get_default_netif();
    if (netif == nullptr || esp_netif_get_ip_info(netif, &ip_info) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get broadcast address");
        return "";
    }
    char broadcast_ip[16];
    uint32_t broadcast_addr = ip_info.ip.addr | ~ip_info.netmask.addr;
    inet_ntoa_r(broadcast_addr, broadcast_ip, sizeof(broadcast_ip));
    return std::string(broadcast_ip);
}

}  // namespace iot