#include "xknob.h"

#define TAG "X-KNOB"

namespace iot {

XKnob::XKnob(SimpleComm *comm) : Thing("XKnob", "智能家居控制旋钮，可以控制小米智能家居：屏幕挂灯，小米电视，除湿机等"), 
               comm_(comm) {
    methods_.AddMethod("TurnOnDevice", "待打开的智能设备", 
        ParameterList({
            Parameter("devive_name", "智能设备的名字", kValueTypeString, true),
        }),
        [this](const ParameterList& parameters) {
            ESP_LOGI(TAG, "trun on");
            std::string device_name = parameters["devive_name"].string();
            std::string json_str = "{";
            json_str += "\"topic\":\"" + device_name + "\",";
            json_str += "\"data\":\"TurnOn\"";
            json_str += "}";
            SendCommand(json_str);
        });
    methods_.AddMethod("TurnOffDevice", "待关闭的智能设备", 
        ParameterList({
            Parameter("devive_name", "智能设备的名字", kValueTypeString, true),
        }),
        [this](const ParameterList& parameters) {
            ESP_LOGI(TAG, "trun off");
            std::string device_name = parameters["devive_name"].string();
            std::string json_str = "{";
            json_str += "\"topic\":\"" + device_name + "\",";
            json_str += "\"data\":\"TurnOff\"";
            json_str += "}";
            SendCommand(json_str);
        });
}

void XKnob::SendCommand(const std::string& cmd) {
    ESP_LOGI(TAG, "<< sending command %s", cmd.c_str());
    if (comm_) {
        ESP_LOGI(TAG, "<< send command %s ", cmd.c_str());
        comm_->Send(cmd);
    }
}

} // namespace iot