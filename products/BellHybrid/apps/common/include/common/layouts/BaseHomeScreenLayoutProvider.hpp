// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <module-utils/EventStore/EventStore.hpp>
#include <utf8/UTF8.hpp>
#include <Temperature.hpp>
#include <time/dateCommon.hpp>

#include <chrono>

namespace app::home_screen
{
    enum class ViewState
    {
        Deactivated,
        DeactivatedWait,
        WaitForConfirmation,
        AlarmEdit,
        ActivatedLowBattery,
        ActivatedWait,
        Activated,
        AlarmRinging,
        AlarmRingingDeactivatedWait,
        AlarmSnoozedWait,
        AlarmSnoozed,
        PreWakeUpActive
    };
}

namespace gui
{
    class Item;
    class SnoozeTimer;

    class BaseHomeScreenLayoutProvider
    {
      private:
        std::string name;

      public:
        explicit BaseHomeScreenLayoutProvider(std::string name) : name{std::move(name)} {};
        virtual ~BaseHomeScreenLayoutProvider() noexcept = default;

        std::string getName()
        {
            return name;
        }

        virtual void setViewState(app::home_screen::ViewState state)            = 0;
        virtual void setTextDescription(const UTF8 &desc)                       = 0;
        virtual void setBatteryLevelState(const Store::Battery &batteryContext) = 0;
        virtual void updateUsbStatus(bool isConnected)                          = 0;
        virtual void setTime(std::time_t newTime)                               = 0;
        virtual void setAlarmTimeFormat(utils::time::Locale::TimeFormat fmt)    = 0;
        [[nodiscard]] virtual std::time_t getAlarmTime() const                  = 0;
        virtual void setAlarmTime(std::time_t newTime)                          = 0;
        virtual SnoozeTimer *getSnoozeTimer()                                   = 0;
        virtual Item *getLayout()                                               = 0;

        virtual void setTemperature(utils::temperature::Temperature newTemp){};
        virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt){};
        virtual void setSnoozeFormat(utils::time::Locale::TimeFormat fmt){};
        virtual void setSnoozeTime(std::time_t newTime){};
        virtual void setQuoteText(const UTF8 &quoteContent, const UTF8 &quoteAuthor){};
    };
} // namespace gui
