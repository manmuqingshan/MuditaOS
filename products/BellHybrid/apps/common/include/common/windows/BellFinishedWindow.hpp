// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/data/BellFinishedWindowSwitchData.hpp>
#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/popups/WindowWithTimer.hpp>

namespace gui
{
    namespace window::bell_finished
    {
        inline constexpr auto defaultName = "BellFinishedWindow";
    } // namespace window::bell_finished

    class Icon;

    class BellFinishedWindow : public WindowWithTimer
    {
      public:
        BellFinishedWindow(app::ApplicationCommon *app, const std::string &name);

      protected:
        void buildInterface() override;
        void exit();
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        Icon *icon{};
        std::string windowToReturn{};
        BellFinishedWindowData::ExitBehaviour exitBehaviour;
    };
} // namespace gui
