// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithAmPm : public HomeScreenLayoutClassic
    {
      public:
        explicit HomeScreenLayoutClassicWithAmPm(std::string name);

        auto setTime(std::time_t newTime) -> void override;

      protected:
        auto buildInterface() -> void override;
        bool isBatteryVisibilityAllowed(const Store::Battery &batteryContext) override;
        void handleContentChanged() override;

        TextFixedSize *fmt{};
    };
} // namespace gui
