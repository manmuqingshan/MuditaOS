// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/options/BellOptionWindow.hpp>

namespace gui
{
    class BellMainMenuWindow : public BellOptionWindow
    {
      public:
        explicit BellMainMenuWindow(app::ApplicationCommon *app);

      private:
        std::list<Option> mainMenuOptionsList();
    };
} // namespace gui
