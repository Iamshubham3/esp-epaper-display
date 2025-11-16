#pragma once

#include <string>

struct WifiCredentials {
  const char* ssid;
  const char* password;
};

void InitWifi(const WifiCredentials& credentials);
