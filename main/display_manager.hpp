#pragma once

#include <array>

extern "C" {
#include "driver/gpio.h"
#include "driver/spi_master.h"
}

#include "price_types.hpp"

class DisplayManager {
 public:
  DisplayManager();

  void InitAll();
  bool ApplyPrice(const PriceUpdate& update);

 private:
  class EpaperDisplay {
   public:
    EpaperDisplay(const char* label, spi_host_device_t host, gpio_num_t cs_pin);
    void Init();
    void ShowPrice(float price_eur);

   private:
    const char* label_;
    spi_host_device_t host_;
    gpio_num_t cs_pin_;
    uint8_t rotation_{0};
  };

  struct DisplaySlot {
    char id;
    EpaperDisplay display;
    float last_price{0.0f};
  };

  std::array<DisplaySlot, 3> slots_;
};
