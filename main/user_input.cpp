#include "user_input.hpp"

#include <cstdio>
#include <string>
#include <utility>

extern "C" {
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}

#include "price_parser.hpp"

namespace {

const char* kUserInputTag = "UserInput";
UserInputHandler s_handler;

void UserInputTask(void* /*param*/) {
  constexpr size_t kBufferSize = 64;
  char buffer[kBufferSize];
  setvbuf(stdin, nullptr, _IONBF, 0);

  while (true) {
    ESP_LOGI(kUserInputTag, "Enter update (e.g., A:12.34) and press ENTER:");
    if (!std::fgets(buffer, kBufferSize, stdin)) {
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    std::string line(buffer);
    PriceUpdate update;
    if (!ParsePricePayload(line, &update)) {
      ESP_LOGW(kUserInputTag, "Invalid input: %s", line.c_str());
      continue;
    }

    if (s_handler) {
      s_handler(update);
    }
  }
}

}  // namespace

void StartUserInputTask(UserInputHandler handler) {
  s_handler = std::move(handler);
  xTaskCreate(&UserInputTask, "user_input_task", 4096, nullptr, 5, nullptr);
}
