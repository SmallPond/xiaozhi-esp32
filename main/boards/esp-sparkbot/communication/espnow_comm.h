#ifndef ESP_NOW_COMM_H
#define ESP_NOW_COMM_H

#include "simple_comm.h"
#include <vector>
#include <string>
#include <cstdint>

namespace iot {

class ESPNowComm : public SimpleComm {
public:
    ESPNowComm(const uint8_t* address = nullptr);
    ~ESPNowComm() override;

    int Init() override;
    int Send(const std::string& str) override;
    void SetRecvCallback(RecvCallback callback) override;

private:
    uint8_t send_address_[6]; // Address to send data to
    /* Default broadcast address */
    const uint8_t defaultsend_address_[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
    bool is_inited_;
};

} // namespace iot

#endif // ESP_NOW_COMM_H