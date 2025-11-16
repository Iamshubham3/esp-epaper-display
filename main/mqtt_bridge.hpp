#pragma once

#include <functional>
#include <string>

#include "price_types.hpp"

using PriceUpdateHandler = std::function<void(const PriceUpdate& update)>;

void InitMqtt(const char* broker_uri, const char* topic, const char* client_id,
              PriceUpdateHandler handler);
void PublishPriceUpdate(const PriceUpdate& update);
bool IsMqttConnected();
