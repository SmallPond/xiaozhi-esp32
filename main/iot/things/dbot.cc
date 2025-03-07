#include "iot/communication/simple_comm.h"
#include <memory>
#include "sdkconfig.h"
#include "iot/thing.h"
#include <esp_log.h>

#define TAG "D-BOT"

namespace iot {

class DBot : public Thing {
public:
    DBot() : Thing("DBot", "多功能两轮平衡车(机器人)，可以精准控制转向的角度和前后移动的距离"), 
             comm_(NULL) {
        methods_.AddMethod("Move", "向前后移动", 
            ParameterList({
                Parameter("distance", "移动的距离，单位为厘米", kValueTypeNumber, true),
            }),
            [this](const ParameterList& parameters) {

            uint32_t distance = static_cast<uint32_t>(parameters["distance"].number());
        
            std::string json_str = "{";
            json_str += "\"action\":\"Move\",";
            json_str += "\"value\":\"" + std::to_string(distance) + "\",";
            json_str += "}";
            SendCommand(json_str);
        });

        methods_.AddMethod("Spin", "旋转", 
            ParameterList({
                Parameter("angle", "旋转的角度", kValueTypeNumber, true),
            }),
            [this](const ParameterList& parameters) {
            uint32_t angle = static_cast<uint32_t>(parameters["angle"].number());
            
            std::string json_str = "{";
            json_str += "\"action\":\" Spin\",";
            json_str += "\"value\":\"" + std::to_string(angle) + "\",";
            json_str += "}";
            SendCommand(json_str);
        });
    }

    int AddComm(SimpleComm *comm) override { 
        if (!comm) {
            return -1;
        }

        if (comm->Init()) {
            return -1;
        }
        comm_ = comm;
        return 0;
    }

    void SendCommand(const std::string& cmd) {
        ESP_LOGI(TAG, "<< send command %s", cmd.c_str());
        if (comm_) {
            comm_->Send(cmd);
        }
    }
private:

    void HandleMessage(const std::vector<uint8_t>& data) {
        printf("Received: %.*s\n", data.size(), data.data());
    }

    SimpleComm* comm_;
};

} // namespace iot

DECLARE_THING(DBot);