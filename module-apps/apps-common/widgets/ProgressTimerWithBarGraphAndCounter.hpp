// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md
#pragma once

#include "ProgressTimer.hpp"
#include "AbstractProgressTime.hpp"
#include <Timers/TimerHandle.hpp>
#include <time/time_conversion.hpp>
#include <atomic>
#include <chrono>
#include <string>

namespace gui
{
    class Text;
    class Progress;
} // namespace gui

namespace app
{
    class ProgressTimerWithBarGraphAndCounter : public ProgressTimer
    {
        gui::Text *text{nullptr};
        gui::Progress *progress{nullptr};
        gui::AbstractProgressTime *timeWidget{nullptr};

        void update() override;
        void updateText();
        void updateTimeWidget();
        void updateProgress();

      public:
        using ProgressTimer::ProgressTimer;

        void attach(gui::Progress *_progress);
        void attach(gui::Text *_text);
        void attach(gui::AbstractProgressTime *_timeWidget);
    };
} // namespace app
