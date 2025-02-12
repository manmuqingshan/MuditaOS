// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Units.hpp>
#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/Image.hpp>

namespace gui
{
    class BellConnectionStatus : public gui::HBox
    {
      public:
        BellConnectionStatus(Item *parent);
        void setFont(const UTF8 &fontName);
        void show(bool visibility);
        bool isVisible() const;

      private:
        Text *statusText = nullptr;
    };
} // namespace gui
