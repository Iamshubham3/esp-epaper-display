#include "mqtt_bridge.hpp"

#include <utility>

extern "C" {
#include "esp_log.h"
#include "mqtt_client.h"
}

#include "price_parser.hpp"

namespace {

const char* kMqttTag = "MQTT";

esp_mqtt_client_handle_t s_client = nullptr;
std::string s_topic;
bool s_connected = false;
PriceUpdateHandler s_handler;

void HandleMqttData(const esp_mqtt_event_t& event) {
  if (!event.topic || event.topic_len <= 0 || !event.data || event.data_len <= 0) {
    return;
  }
  const std::string topic(event.topic, event.topic_len);
  if (topic != s_topic) {
    return;
  }
  const std::string payload(event.data, event.data_len);

  PriceUpdate update;
  if (!ParsePricePayload(payload, &update)) {
    ESP_LOGW(kMqttTag, "Failed to parse payload: %s", payload.c_str());
    return;
  }

  if (s_handler) {
    s_handler(update);
  }
}

void MqttEventHandler(void* /*handler_args*/, esp_event_base_t /*base*/, int32_t event_id,
                      void* event_data) {
  auto* event = static_cast<esp_mqtt_event_handle_t>(event_data);
  switch (event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(kMqttTag, "Connected to broker");
      s_connected = true;
      esp_mqtt_client_subscribe(event->client, s_topic.c_str(), 1);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGW(kMqttTag, "Disconnected from broker");
      s_connected = false;
      break;
    case MQTT_EVENT_DATA:
      HandleMqttData(*event);
      break;
    default:
      break;
  }
}

}  // namespace

void InitMqtt(const char* broker_uri, const char* topic, const char* client_id,
              PriceUpdateHandler handler) {
  s_topic = topic ? topic : "";
  s_handler = std::move(handler);

  esp_mqtt_client_config_t mqtt_cfg = {};
  mqtt_cfg.broker.address.uri = broker_uri;
  mqtt_cfg.credentials.client_id = client_id;

  s_client = esp_mqtt_client_init(&mqtt_cfg);
  esp_mqtt_client_register_event(s_client, MQTT_EVENT_ANY, &MqttEventHandler, nullptr);
  esp_mqtt_client_start(s_client);
}

void PublishPriceUpdate(const PriceUpdate& update) {
  if (!s_client || !s_connected) {
    ESP_LOGW(kMqttTag, "MQTT not ready for publishing");
    return;
  }
  const std::string payload = BuildPricePayload(update);
  esp_mqtt_client_publish(s_client, s_topic.c_str(), payload.c_str(), 0, 1, true);
}

bool IsMqttConnected() {
  return s_connected;
}
