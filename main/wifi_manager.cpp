#include "wifi_manager.hpp"

#include <cstring>

extern "C" {
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
}

namespace {

constexpr int kWifiConnectedBit = BIT0;
EventGroupHandle_t s_wifi_event_group = nullptr;
const char* kWifiTag = "WiFi";

void WifiEventHandler(void* /*arg*/, esp_event_base_t event_base, int32_t event_id,
                      void* /*event_data*/) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
    ESP_LOGW(kWifiTag, "Wi-Fi disconnected, retrying...");
    esp_wifi_connect();
    xEventGroupClearBits(s_wifi_event_group, kWifiConnectedBit);
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(kWifiTag, "Wi-Fi connected");
    xEventGroupSetBits(s_wifi_event_group, kWifiConnectedBit);
  }
}

}  // namespace

void InitWifi(const WifiCredentials& credentials) {
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(
      esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, nullptr,
                                          nullptr));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                      &WifiEventHandler, nullptr, nullptr));

  wifi_config_t wifi_config = {};
  std::strncpy(reinterpret_cast<char*>(wifi_config.sta.ssid), credentials.ssid,
               sizeof(wifi_config.sta.ssid));
  wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
  std::strncpy(reinterpret_cast<char*>(wifi_config.sta.password), credentials.password,
               sizeof(wifi_config.sta.password));
  wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';
  wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(kWifiTag, "Waiting for Wi-Fi connection...");
  xEventGroupWaitBits(s_wifi_event_group, kWifiConnectedBit, pdFALSE, pdFALSE, portMAX_DELAY);
}
