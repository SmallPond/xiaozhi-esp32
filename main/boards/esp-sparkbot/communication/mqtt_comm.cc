#include "mqtt_comm.h"
#include "esp_log.h"
#include "esp_mqtt.h"
#include "cJSON.h"

namespace iot {

MqttComm::MqttComm(const std::string& broker_uri, const std::string& client_id)
    : broker_uri_(broker_uri), client_id_(client_id), client_(nullptr), is_inited__(false) {}

MqttComm::~MqttComm() {
	if (client_) {
		esp_mqtt_client_destroy(client_);
	}
}

int MqttComm::Init() {
	if (is_inited__) {
		ESP_LOGI("MqttComm", "Already initialized");
		return 0;
	}

	esp_mqtt_client_config_t mqtt_cfg = {};
	mqtt_cfg.broker.address.uri = broker_uri_.c_str();
	mqtt_cfg.credentials.client_id = client_id_.c_str();

	client_ = esp_mqtt_client_init(&mqtt_cfg);
	if (!client_) {
		ESP_LOGE("MqttComm", "Failed to initialize MQTT client");
		return -1;
	}

	esp_mqtt_client_register_event(client_, static_cast<esp_mqtt_event_id_t>(ESP_EVENT_ANY_ID),
	                               MqttEventHandler, this);
	esp_mqtt_client_start(client_);

	is_inited__ = true;
	ESP_LOGI("MqttComm", "Initialization successful");
	return 0;
}

int MqttComm::Send(const std::string& json_str) {
	
	if (!is_inited__) {
		ESP_LOGW("MqttComm", "Not initialized, attempting to initialize");
		if (Init() != 0) {
			ESP_LOGE("MqttComm", "Failed to initialize during Send");
			return -1;
		}
	}

	// 解析 JSON 数据
	cJSON* json = cJSON_Parse(json_str.c_str());
	if (!json) {
		ESP_LOGE("MqttComm", "Failed to parse JSON data");
		return -1;
	}

	// 提取 topic 和 data
	cJSON* topic_json = cJSON_GetObjectItem(json, "topic");
	cJSON* data_json = cJSON_GetObjectItem(json, "data");

	if (!topic_json || !data_json || !cJSON_IsString(topic_json) || !cJSON_IsString(data_json)) {
		ESP_LOGE("MqttComm", "Invalid JSON format: missing or invalid 'topic' or 'data'");
		cJSON_Delete(json);
		return -1;
	}

	const char* topic = topic_json->valuestring;
	const char* data = data_json->valuestring;

	// 发布消息
	int msg_id = esp_mqtt_client_publish(client_, topic, data, strlen(data), 1, 0);
	if (msg_id < 0) {
		ESP_LOGE("MqttComm", "Failed to publish message");
		cJSON_Delete(json);
		return -1;
	}

	ESP_LOGI("MqttComm", "Message published successfully to topic: %s", topic);
	cJSON_Delete(json);
	return 0;
}

void MqttComm::SetRecvCallback(RecvCallback callback) {
	recv_callback_ = callback;
}

void MqttComm::MqttEventHandler(void* handler_args, esp_event_base_t base,
                                int32_t event_id, void* event_data) {
	MqttComm* instance = static_cast<MqttComm*>(handler_args);
	esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);

	switch (event->event_id) {
		case MQTT_EVENT_DATA:
			if (instance->recv_callback_) {
				std::vector<uint8_t> data(event->data, event->data + event->data_len);
				instance->recv_callback_(data);
			}
			break;
		default:
			break;
	}
}

}  // namespace iot