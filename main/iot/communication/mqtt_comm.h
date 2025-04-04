#ifndef IOT_MQTT_COMM_H_
#define IOT_MQTT_COMM_H_

#include "simple_comm.h"
#include "esp_mqtt.h"
#include <string>
#include "cJSON.h"

namespace iot {

class MqttComm : public SimpleComm {
 public:
	MqttComm(const std::string& broker_uri, const std::string& client_id);
	~MqttComm() override;

	int Init() override;
	int Send(const std::string& json_str) override; // 传入 JSON 字符串
	void SetRecvCallback(RecvCallback callback) override;

 private:
	std::string broker_uri_;
	std::string client_id_;
	esp_mqtt_client_handle_t client_;
	RecvCallback recv_callback_;
	bool is_inited__;

	static void MqttEventHandler(void* handler_args, esp_event_base_t base,
	                             int32_t event_id, void* event_data);
};

}  // namespace iot

#endif  // IOT_MQTT_COMM_H_