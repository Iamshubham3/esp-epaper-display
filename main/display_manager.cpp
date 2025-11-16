#include "display_manager.hpp"

#include <cstdio>

extern "C" {
#include "esp_log.h"
}

namespace {
const char* kDisplayTag = "Display";
}

DisplayManager::EpaperDisplay::EpaperDisplay(const char* label, spi_host_device_t host,
                                             gpio_num_t cs_pin)
    : label_(label), host_(host), cs_pin_(cs_pin) {}

void DisplayManager::EpaperDisplay::Init() {
  ESP_LOGI(kDisplayTag, "[%s] init host=%d cs=%d", label_, host_, cs_pin_);
  rotation_ = 1;
}

void DisplayManager::EpaperDisplay::ShowPrice(float price_eur) {
  char buffer[16];
  std::snprintf(buffer, sizeof(buffer), "%.2f", price_eur);
  ESP_LOGI(kDisplayTag, "[%s] price -> %s EUR", label_, buffer);
}

DisplayManager::DisplayManager()
    : slots_{DisplaySlot{'A', EpaperDisplay("Display 1", SPI2_HOST, GPIO_NUM_5)},
             DisplaySlot{'B', EpaperDisplay("Display 2", SPI2_HOST, GPIO_NUM_0)},
             DisplaySlot{'C', EpaperDisplay("Display 3", SPI2_HOST, GPIO_NUM_2)}} {}

void DisplayManager::InitAll() {
  for (auto& slot : slots_) {
    slot.display.Init();
  }
}

bool DisplayManager::ApplyPrice(const PriceUpdate& update) {
  const char normalized = NormalizeDisplayId(update.id);
  for (auto& slot : slots_) {
    if (slot.id == normalized) {
      slot.last_price = update.price;
      slot.display.ShowPrice(update.price);
      return true;
    }
  }

  ESP_LOGW(kDisplayTag, "Unknown display id %c", normalized);
  return false;
}
