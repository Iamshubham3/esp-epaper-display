#include "price_parser.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

namespace {

std::string Trim(const std::string& input) {
  const auto start = input.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return {};
  }
  const auto end = input.find_last_not_of(" \t\r\n");
  return input.substr(start, end - start + 1);
}

}  // namespace

bool ParsePricePayload(const std::string& raw, PriceUpdate* out_update) {
  if (!out_update) {
    return false;
  }

  const std::string payload = Trim(raw);
  if (payload.empty()) {
    return false;
  }

  PriceUpdate update;
  update.id = NormalizeDisplayId(payload[0]);

  size_t value_start = 1;
  while (value_start < payload.size() &&
         (payload[value_start] == ':' || payload[value_start] == ' ')) {
    ++value_start;
  }
  if (value_start >= payload.size()) {
    return false;
  }

  char* end_ptr = nullptr;
  update.price = std::strtof(payload.data() + value_start, &end_ptr);
  if (end_ptr == payload.data() + value_start) {
    return false;
  }

  *out_update = update;
  return true;
}

std::string BuildPricePayload(const PriceUpdate& update) {
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%c:%.2f", update.id, update.price);
  return std::string(buffer);
}
