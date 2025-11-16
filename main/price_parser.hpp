#pragma once

#include <string>

#include "price_types.hpp"

bool ParsePricePayload(const std::string& raw, PriceUpdate* out_update);
std::string BuildPricePayload(const PriceUpdate& update);
