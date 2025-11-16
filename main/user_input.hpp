#pragma once

#include <functional>

#include "price_types.hpp"

using UserInputHandler = std::function<void(const PriceUpdate& update)>;

void StartUserInputTask(UserInputHandler handler);
