#ifndef UDP_COMM_H_
#define UDP_COMM_H_

#include "simple_comm.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_netif.h"
#include <string>
#include <vector>
#include <functional>

namespace iot {

// 默认绑定端口
#define DEFAULT_UDP_PORT 6060
// IoT Port
#define DEFAULT_TARGET_PORT 6090
class UDPComm : public SimpleComm {
public:
    explicit UDPComm(const std::string& target_ip = "", int target_port = DEFAULT_TARGET_PORT);
    ~UDPComm() override;

    int Init() override;
    int Send(const std::string& str) override;
    void SetRecvCallback(RecvCallback callback) override;

private:
    // static void RecvTask(void* arg);

    std::string GetLocalIpAddress();
    std::string GetBroadcastAddress(const std::string& local_ip);

    int sock_;
    std::string target_ip_;
    std::string bind_ip_;
    int16_t bind_port_;
    int16_t target_port_;
    RecvCallback recv_callback_;
    bool is_inited = false;
};

}  // namespace iot

#endif  // UDP_COMM_H_