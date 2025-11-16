#include "display_manager.hpp"
#include "mqtt_bridge.hpp"
#include "price_types.hpp"
#include "user_input.hpp"
#include "wifi_manager.hpp"

extern "C" {
#include "esp_err.h"
#include "nvs_flash.h"
}

namespace {

constexpr char kWifiSsid[] = "CLUB-96450";
constexpr char kWifiPassword[] = "95889800581528528105";
constexpr char kMqttUri[] = "mqtt://192.168.178.52";
constexpr char kMqttTopic[] = "Price";
constexpr char kMqttClientId[] = "esp32-epaper";

DisplayManager g_display_manager;

}  // namespace

extern "C" void app_main(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
  } else {
    ESP_ERROR_CHECK(ret);
  }

  InitWifi(WifiCredentials{kWifiSsid, kWifiPassword});
  g_display_manager.InitAll();

  InitMqtt(kMqttUri, kMqttTopic, kMqttClientId,
           [](const PriceUpdate& update) { g_display_manager.ApplyPrice(update); });

  StartUserInputTask([](const PriceUpdate& update) {
    if (g_display_manager.ApplyPrice(update)) {
      PublishPriceUpdate(update);
    }
  });
}
