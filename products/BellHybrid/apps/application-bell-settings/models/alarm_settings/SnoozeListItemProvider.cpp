// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "SnoozeListItemProvider.hpp"
#include "common/models/AbstractAudioModel.hpp"
#include <common/widgets/list_items/NumberWithSuffix.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <common/widgets/list_items/NumericWithBar.hpp>
#include <common/widgets/ListItems.hpp>
#include <SongsListViewItem.hpp>
#include <LanguageUtils.hpp>

#include <utility>

namespace app::bell_settings
{
    using namespace gui;

    list_items::NumberWithSuffix::spinner_type::range getDefaultChimeIntervalRange()
    {
        return {0, 1, 2, 3, 5};
    }

    list_items::NumberWithSuffix::spinner_type::range transformChimeIntervalsRange(const std::uint32_t chimeLength)
    {
        auto chimeIntervals = getDefaultChimeIntervalRange();

        chimeIntervals.erase(std::remove_if(chimeIntervals.begin(),
                                            chimeIntervals.end(),
                                            [chimeLength](const auto &e) { return e >= chimeLength; }),
                             chimeIntervals.end());

        return chimeIntervals;
    }

    std::optional<list_items::NumberWithSuffix::spinner_type::value_type> calculateCurrentChimeIntervalValue(
        const list_items::NumberWithSuffix::spinner_type::range &range,
        const list_items::NumberWithSuffix::spinner_type::value_type &chimeInterval)
    {
        if (range.size() == 1) {
            return {};
        }

        if (chimeInterval >= range.back()) {
            return range.back();
        }
        return chimeInterval;
    }

    SnoozeListItemProvider::SnoozeListItemProvider(AbstractSnoozeSettingsModel &settingsModel,
                                                   std::shared_ptr<SongsModel> songsModel)
        : settingsModel{settingsModel}, songsModel{std::move(songsModel)}
    {
        buildListItems();
    }

    void SnoozeListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 5U;
        internalData.reserve(itemCount);

        auto onOff = new OnOffListItem(settingsModel.getSnoozeOnOff(),
                                       utils::translate("app_bell_settings_alarm_settings_snooze"));

        internalData.emplace_back(onOff);
        constexpr auto snoozeLengthStep = 1U;
        constexpr auto snoozeLengthMin  = 1U;
        constexpr auto snoozeLengthMax  = 30U;

        auto chimeLengthBottomDescription =
            utils::language::getCorrectMinutesNumeralForm(settingsModel.getSnoozeLength().getValue());
        auto chimeLength = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{snoozeLengthMin, snoozeLengthMax, snoozeLengthStep},
            settingsModel.getSnoozeLength(),
            utils::translate("app_bell_settings_alarm_settings_snooze_length"),
            chimeLengthBottomDescription);

        chimeLength->set_on_value_change_cb([chimeLength](const auto &val) {
            chimeLength->setBottomDescriptionText(utils::language::getCorrectMinutesNumeralForm(val));
        });

        chimeLength->onEnter = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
            }
        };

        internalData.emplace_back(chimeLength);

        auto chimeInterval = new list_items::NumberWithSuffix(
            getDefaultChimeIntervalRange(),
            settingsModel.getSnoozeChimeInterval(),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval"),
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_interval_bot_desc"));
        chimeLength->setValue();

        internalData.emplace_back(chimeInterval);

        chimeInterval->onEnter = [chimeInterval, chimeLength, this]() {
            if (chimeLength != nullptr) {
                const auto currentChimeLength   = chimeLength->value();
                const auto currentChimeInterval = chimeInterval->value();
                const auto calculatedRange      = transformChimeIntervalsRange(currentChimeLength);
                const auto calculatedChimeInterval =
                    calculateCurrentChimeIntervalValue(calculatedRange, currentChimeInterval);

                if (calculatedChimeInterval) {
                    chimeInterval->set_range(calculatedRange);
                    chimeInterval->set_value(calculatedChimeInterval.value());
                }
                else {
                    this->onExit();
                }
            }
            return false;
        };

        chimeInterval->onProceed = [chimeInterval, this]() {
            if (chimeInterval->value() == 0) {
                this->onExit();
                return true;
            }
            return false;
        };

        auto snoozeChimeTone =
            new SongsListViewItem(utils::translate("app_bell_settings_alarm_settings_snooze_chime_tone"),
                                  settingsModel.getSnoozeChimeTone(),
                                  songsModel);

        currentSoundPath = settingsModel.getSnoozeChimeTone().getValue();
        snoozeChimeTone->set_on_value_change_cb([this](const auto &val) {
            currentSoundPath = val;
            if (onToneChange) {
                onToneChange(currentSoundPath);
            }
        });
        snoozeChimeTone->onEnter = [this, chimeInterval]() {
            if (chimeInterval->value() == 0) {
                this->onExit();
                return;
            }
            if (onToneEnter) {
                onToneEnter(currentSoundPath);
            }
        };
        snoozeChimeTone->onExit = [this]() {
            if (onToneExit) {
                onToneExit(currentSoundPath);
            }
        };
        snoozeChimeTone->onProceed = [this]() {
            if (onToneProceed) {
                return onToneProceed(currentSoundPath);
            }
            return false;
        };
        internalData.emplace_back(snoozeChimeTone);

        constexpr auto volumeStep = 1U;
        constexpr auto volumeMin  = AbstractAudioModel::minVolume;
        constexpr auto volumeMax  = AbstractAudioModel::maxVolume;
        snoozeChimeVolume         = new list_items::NumericWithBar(
            list_items::NumericWithBar::spinner_type::range{volumeMin, volumeMax, volumeStep},
            settingsModel.getSnoozeChimeVolume(),
            volumeMax,
            utils::translate("app_bell_settings_alarm_settings_snooze_chime_volume"));
        snoozeChimeVolume->set_on_value_change_cb([this](const auto &val) {
            if (onVolumeChange) {
                onVolumeChange(val);
            }
        });

        snoozeChimeVolume->onEnter = [this, snoozeChimeTone]() {
            if (onVolumeEnter) {
                onVolumeEnter(snoozeChimeTone->value());
            }
        };

        snoozeChimeVolume->onExit = [this]() {
            if (onVolumeExit) {
                onVolumeExit(snoozeChimeVolume->value());
            }
        };

        internalData.emplace_back(snoozeChimeVolume);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto SnoozeListItemProvider::getCurrentVolume() -> std::uint8_t
    {
        return snoozeChimeVolume->value();
    }
} // namespace app::bell_settings
