// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/InternalModel.hpp>
#include <Temperature.hpp>
#include <time/time_locale.hpp>
#include <date/date.h>

namespace gui
{
    class DateSetListItem;
    class TimeSetListItem;
    class TimeFormatSetListItem;
    class DateFormatSetListItem;
    class TemperatureUnitListItem;
} // namespace gui

namespace app::bell_settings
{
    class DateTimeUnitsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
    {
      public:
        explicit DateTimeUnitsModel(app::ApplicationCommon *app);
        ~DateTimeUnitsModel();

        auto clearData() -> void;
        auto saveData() -> void;
        virtual auto loadData() -> void;
        virtual auto isDateChanged() -> bool;
        auto createData() -> void;
        auto requestRecords(std::uint32_t offset, std::uint32_t limit) -> void override;
        [[nodiscard]] auto getItem(gui::Order order) -> gui::ListItem * override;
        [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
        [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
        [[nodiscard]] auto getTemperatureUnit() const -> utils::temperature::Temperature::Unit;
        [[nodiscard]] auto getTimeFormat() const -> utils::time::Locale::TimeFormat;
        auto setTemperatureUnit(utils::temperature::Temperature::Unit unit) -> void;

      protected:
        app::ApplicationCommon *application{};
        gui::DateSetListItem *yearSetListItem{};
        gui::DateSetListItem *monthSetListItem{};
        gui::DateSetListItem *daySetListItem{};
        gui::TimeSetListItem *timeSetListItem{};
        gui::DateFormatSetListItem *dateFmtSetListItem{};
        gui::TimeFormatSetListItem *timeFmtSetListItem{};
        gui::TemperatureUnitListItem *temperatureUnitListItem{};

        void sendRtcUpdateTimeMessage(std::time_t newTime);
        void sendTimeFmtUpdateMessage(utils::time::Locale::TimeFormat newFmt);
        void sendDateFmtUpdateMessage(utils::time::Locale::DateFormat newFmt);

      private:
        date::year_month_day dateLoaded{};
    };

    class DateTimeUnitsModelFactoryResetValues : public DateTimeUnitsModel
    {
      public:
        using DateTimeUnitsModel::DateTimeUnitsModel;
        auto loadData() -> void override;
        auto isDateChanged() -> bool override;
    };
} // namespace app::bell_settings
