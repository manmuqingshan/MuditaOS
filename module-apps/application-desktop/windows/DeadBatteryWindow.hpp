// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class Image;
    class DeadBatteryWindow : public AppWindow
    {
      public:
        DeadBatteryWindow(app::ApplicationCommon *app, const std::string &name);
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
    };
} /* namespace gui */
