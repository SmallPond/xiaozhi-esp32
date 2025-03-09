#include "dbot.h"

#define TAG "D-BOT"

namespace iot {

DBot::DBot(SimpleComm *comm) : Thing("DBot", "多功能两轮平衡车(机器人)，可以精准控制转向的角度和前后移动的距离"), 
               comm_(comm) {
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

void DBot::SendCommand(const std::string& cmd) {
    ESP_LOGI(TAG, "<< sending command %s", cmd.c_str());
    if (comm_) {
        ESP_LOGI(TAG, "<< send command %s ", cmd.c_str());
        comm_->Send(cmd);
    }
}

} // namespace iot