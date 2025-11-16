#pragma once

#include <cctype>

struct PriceUpdate {
  char id{'A'};
  float price{0.0f};
};

constexpr char NormalizeDisplayId(char id) {
  return static_cast<char>(std::toupper(static_cast<unsigned char>(id)));
}
