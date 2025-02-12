﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ApplicationBellSettings.hpp"
#include "presenter/TimeUnitsPresenter.hpp"
#include "presenter/LayoutWindowPresenter.hpp"
#include "models/TemperatureUnitModel.hpp"
#include "models/AboutYourBellModel.hpp"
#include "models/AudioErrorModel.hpp"
#include "models/RelaxationListItemProvider.hpp"
#include "models/BedsideListItemProvider.hpp"
#include "models/alarm_settings/AlarmSettingsListItemProvider.hpp"
#include "models/alarm_settings/PrewakeUpListItemProvider.hpp"
#include "models/alarm_settings/BedtimeSettingsListItemProvider.hpp"
#include "models/alarm_settings/PrewakeUpSettingsModel.hpp"
#include "models/alarm_settings/SnoozeListItemProvider.hpp"
#include "models/alarm_settings/SnoozeSettingsModel.hpp"
#include "models/alarm_settings/AlarmSettingsModel.hpp"
#include "presenter/BedtimeSettingsPresenter.hpp"
#include "presenter/AboutYourBellWindowPresenter.hpp"
#include "presenter/alarm_settings/SnoozePresenter.hpp"
#include "presenter/FrontlightPresenter.hpp"
#include "presenter/ShortcutsWindowPresenter.hpp"
#include "presenter/UpdateInstructionWindowPresenter.hpp"
#include "presenter/BedsidePresenter.hpp"
#include "windows/AboutYourBellWindow.hpp"
#include "windows/BellSettingsLanguageWindow.hpp"
#include "windows/BellSettingsLayoutWindow.hpp"
#include "windows/BellSettingsFrontlightWindow.hpp"
#include "windows/BellSettingsFactoryResetWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsMenuWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsSnoozeWindow.hpp"
#include "windows/alarm_settings/BellSettingsAlarmSettingsWindow.hpp"
#include "windows/alarm_settings/BellSettingsPrewakeUpWindow.hpp"
#include "windows/BellSettingsBedtimeToneWindow.hpp"
#include "windows/BellSettingsRelaxationWindow.hpp"
#include "windows/BellSettingsHomeViewWindow.hpp"
#include "windows/BellSettingsTimeUnitsWindow.hpp"
#include "windows/BellSettingsWindow.hpp"
#include "windows/BellSettingsBedsideWindow.hpp"

#include <Paths.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <common/BellPowerOffPresenter.hpp>
#include <common/models/BedtimeModel.hpp>
#include <common/models/BedsideModel.hpp>
#include <common/models/LayoutModel.hpp>
#include <common/models/RelaxationFadeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/windows/ShortcutsWindow.hpp>
#include <common/windows/UpdateInstructionWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/models/AudioModel.hpp>
#include <common/models/TimeModel.hpp>
#include <common/models/SongsModel.hpp>
#include <service-appmgr/messages/GetCurrentDisplayLanguageResponse.hpp>

namespace app
{
    ApplicationBellSettings::ApplicationBellSettings(std::string name,
                                                     std::string parent,
                                                     StatusIndicators statusIndicators,
                                                     StartInBackground startInBackground,
                                                     std::uint32_t stackDepth)
        : Application(std::move(name), std::move(parent), statusIndicators, startInBackground, stackDepth),
          audioModel{std::make_unique<AudioModel>(this)}
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
    }

    sys::ReturnCodes ApplicationBellSettings::InitHandler()
    {
        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }
        createUserInterface();

        connect(typeid(manager::GetCurrentDisplayLanguageResponse), [&]([[maybe_unused]] sys::Message *msg) {
            if (gui::window::name::bellSettingsLanguage == getCurrentWindow()->getName()) {
                switchWindow(gui::window::bell_finished::defaultName,
                             gui::BellFinishedWindowData::Factory::create("circle_success_big",
                                                                          gui::window::name::bellSettings));
                return sys::msgHandled();
            }
            return sys::msgNotHandled();
        });

        return sys::ReturnCodes::Success;
    }

    void ApplicationBellSettings::createUserInterface()
    {
        windowsFactory.attach(gui::name::window::main_window, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellSettingsWindow>(app);
        });

        windowsFactory.attach(
            gui::window::name::bellSettingsLanguage, [&](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bell_settings::LanguageWindowPresenter>(this);
                return std::make_unique<gui::BellSettingsLanguageWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsLayout, [this](ApplicationCommon *app, const std::string &name) {
                auto layoutModel = std::make_unique<bell_settings::LayoutModel>(this);
                auto timeModel   = std::make_unique<app::TimeModel>();
                auto quoteModel  = std::make_unique<app::QuoteModel>(this);
                auto presenter   = std::make_unique<bell_settings::LayoutWindowPresenter>(
                    this, std::move(layoutModel), std::move(timeModel), std::move(quoteModel));
                return std::make_unique<gui::BellSettingsLayoutWindow>(app, std::move(presenter), name);
            });

        windowsFactory.attach(gui::window::name::bellSettingsRelaxation,
                              [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                                  auto fadeModel = std::make_unique<RelaxationFadeModel>(app);
                                  auto provider =
                                      std::make_shared<bell_settings::RelaxationListItemProvider>(std::move(fadeModel));
                                  auto presenter = std::make_unique<bell_settings::RelaxationWindowPresenter>(provider);
                                  return std::make_unique<gui::BellSettingsRelaxationWindow>(app, std::move(presenter));
                              });

        windowsFactory.attach(
            gui::BellSettingsFrontlightWindow::name, [](ApplicationCommon *app, const std::string &name) {
                auto model    = std::make_unique<bell_settings::FrontlightModel>(app);
                auto provider = std::make_shared<bell_settings::FrontlightListItemProvider>(*model);
                auto presenter =
                    std::make_unique<bell_settings::FrontlightPresenter>(std::move(provider), std::move(model));
                return std::make_unique<gui::BellSettingsFrontlightWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::BellSettingsBedsideWindow::name, [](ApplicationCommon *app, const std::string &name) {
                auto frontlightModel = std::make_unique<bell_settings::FrontlightModel>(app);
                auto brightnessModel = std::make_unique<bell_settings::BedsideBrightnessModel>(app);
                auto timeModel       = std::make_unique<bell_settings::BedsideTimeModel>(app);
                auto bedsideModel =
                    std::make_unique<bell_settings::BedsideModel>(std::move(brightnessModel), std::move(timeModel));
                auto provider  = std::make_shared<bell_settings::BedsideListItemProvider>(std::move(bedsideModel),
                                                                                         std::move(frontlightModel));
                auto presenter = std::make_unique<bell_settings::BedsidePresenter>(std::move(provider));
                return std::make_unique<gui::BellSettingsBedsideWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::window::bell_finished::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellFinishedWindow>(app, name);
                              });

        windowsFactory.attach(gui::window::name::bellSettingsHomeView,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsHomeViewWindow>(app);
                              });

        windowsFactory.attach(
            gui::window::name::bellSettingsBedtimeTone, [this](ApplicationCommon *app, const std::string &name) {
                auto bedtimeModel = std::make_shared<bell_bedtime::BedtimeModel>(app, *audioModel);

                const auto &pathSorting =
                    SoundsRepository::PathSorting{paths::audio::proprietary() / paths::audio::bedtimeReminder(),
                                                  SoundsRepository::SortingBy::TitleAscending};

                auto soundsRepository = std::make_unique<SoundsRepository>(this, pathSorting);
                auto songsModel       = std::make_unique<SongsModel>(this, std::move(soundsRepository));

                auto provider        = std::make_unique<bell_settings::BedtimeSettingsListItemProvider>(bedtimeModel,
                                                                                                 std::move(songsModel));
                auto audioErrorModel = std::make_unique<bell_settings::AudioErrorModel>();
                auto presenter       = std::make_unique<bell_settings::SettingsPresenter>(
                    app, std::move(provider), bedtimeModel, *audioModel, std::move(audioErrorModel));
                return std::make_unique<gui::BellSettingsBedtimeToneWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::window::name::bellSettingsTimeUnits, [this](ApplicationCommon *app, const std::string &name) {
                auto layoutModel          = std::make_unique<bell_settings::LayoutModel>(this);
                auto temperatureUnitModel = std::make_unique<bell_settings::TemperatureUnitModel>(app);
                auto timeUnitsProvider    = std::make_shared<bell_settings::DateTimeUnitsModel>(app);
                auto presenter            = std::make_unique<bell_settings::TimeUnitsWindowPresenter>(
                    this, timeUnitsProvider, std::move(temperatureUnitModel), std::move(layoutModel));
                return std::make_unique<gui::BellSettingsTimeUnitsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(gui::BellTurnOffOptionWindow::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellTurnOffOptionWindow>(app);
                              });

        windowsFactory.attach(gui::BellTurnOffWindow::name, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::BellTurnOffWindow>(app, std::make_unique<gui::BellPowerOffPresenter>(app));
        });

        windowsFactory.attach(gui::BellSettingsFactoryResetWindow::defaultName,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsFactoryResetWindow>(app);
                              });

        windowsFactory.attach(
            gui::BellSettingsPrewakeUpWindow::name, [this](ApplicationCommon *app, const std::string &name) {
                auto prewakeUpChimeDurationModel = std::make_unique<bell_settings::PrewakeUpChimeDurationModel>(this);
                auto prewakeUpChimeToneModel     = std::make_unique<bell_settings::PrewakeUpChimeToneModel>(this);
                auto prewakeUpChimeVolumeModel =
                    std::make_unique<bell_settings::PrewakeUpChimeVolumeModel>(*audioModel);
                auto prewakeUpLightDurationModel = std::make_unique<bell_settings::PrewakeUpLightDurationModel>(this);
                auto prewakeUpFrontlightModel    = std::make_unique<bell_settings::PrewakeUpFrontlightModel>(this);
                auto prewakeUpSettingsModel =
                    std::make_unique<bell_settings::PrewakeUpSettingsModel>(std::move(prewakeUpChimeDurationModel),
                                                                            std::move(prewakeUpChimeToneModel),
                                                                            std::move(prewakeUpChimeVolumeModel),
                                                                            std::move(prewakeUpLightDurationModel),
                                                                            std::move(prewakeUpFrontlightModel));

                const auto &pathSorting =
                    SoundsRepository::PathSorting{paths::audio::proprietary() / paths::audio::preWakeup(),
                                                  SoundsRepository::SortingBy::TrackIdAscending};
                auto soundsRepository = std::make_unique<SoundsRepository>(this, pathSorting);
                auto songsModel       = std::make_unique<SongsModel>(this, std::move(soundsRepository));

                auto provider = std::make_unique<bell_settings::PrewakeUpListItemProvider>(*prewakeUpSettingsModel,
                                                                                           std::move(songsModel));

                auto frontlightModel = std::make_unique<bell_settings::FrontlightModel>(app);
                auto audioErrorModel = std::make_unique<bell_settings::AudioErrorModel>();
                auto presenter =
                    std::make_unique<bell_settings::PrewakeUpWindowPresenter>(app,
                                                                              std::move(provider),
                                                                              std::move(prewakeUpSettingsModel),
                                                                              *audioModel,
                                                                              std::move(frontlightModel),
                                                                              std::move(audioErrorModel));
                return std::make_unique<gui::BellSettingsPrewakeUpWindow>(app, std::move(presenter));
            });

        // Alarm setup
        windowsFactory.attach(gui::BellSettingsAlarmSettingsMenuWindow::name,
                              [](ApplicationCommon *app, const std::string &name) {
                                  return std::make_unique<gui::BellSettingsAlarmSettingsMenuWindow>(app);
                              });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsSnoozeWindow::name,
            [this](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto snoozeOnOffModel         = std::make_unique<bell_settings::SnoozeOnOffModel>(this);
                auto snoozeLengthModel        = std::make_unique<bell_settings::SnoozeLengthModel>(this);
                auto snoozeChimeIntervalModel = std::make_unique<bell_settings::SnoozeChimeIntervalModel>(this);
                auto snoozeChimeToneModel     = std::make_unique<bell_settings::SnoozeChimeToneModel>(this);
                auto snoozeChimeVolumeModel   = std::make_unique<bell_settings::SnoozeChimeVolumeModel>(*audioModel);
                auto snoozeSettingsModel =
                    std::make_unique<bell_settings::SnoozeSettingsModel>(std::move(snoozeOnOffModel),
                                                                         std::move(snoozeLengthModel),
                                                                         std::move(snoozeChimeIntervalModel),
                                                                         std::move(snoozeChimeToneModel),
                                                                         std::move(snoozeChimeVolumeModel));

                const auto &pathSorting =
                    SoundsRepository::PathSorting{paths::audio::proprietary() / paths::audio::snooze(),
                                                  SoundsRepository::SortingBy::TrackIdAscending};
                auto soundsRepository = std::make_unique<SoundsRepository>(this, pathSorting);
                auto songsModel       = std::make_unique<SongsModel>(this, std::move(soundsRepository));
                auto provider         = std::make_unique<bell_settings::SnoozeListItemProvider>(*snoozeSettingsModel,
                                                                                        std::move(songsModel));
                auto audioErrorModel  = std::make_unique<bell_settings::AudioErrorModel>();

                auto presenter = std::make_unique<bell_settings::SnoozePresenter>(
                    app, std::move(provider), std::move(snoozeSettingsModel), *audioModel, std::move(audioErrorModel));
                return std::make_unique<gui::BellSettingsAlarmSettingsSnoozeWindow>(app, std::move(presenter));
            });
        windowsFactory.attach(
            gui::BellSettingsAlarmSettingsWindow::name,
            [this](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                const auto &pathSorting = std::vector<SoundsRepository::PathSorting>{
                    {paths::audio::proprietary() / paths::audio::alarm(),
                     SoundsRepository::SortingBy::TrackIdAscending},
                    {paths::audio::userApp() / paths::audio::alarm(), SoundsRepository::SortingBy::TitleAscending}};
                const app::LabelsWithPaths labelsWithPaths{
                    {"app_bell_settings_alarm_settings_sounds", paths::audio::proprietary() / paths::audio::alarm()},
                    {"app_bell_settings_alarm_settings_custom_sounds",
                     paths::audio::userApp() / paths::audio::alarm()}};
                auto soundsRepository = std::make_unique<SoundsRepository>(this, pathSorting);
                auto songsModel = std::make_unique<SongsModel>(this, std::move(soundsRepository), labelsWithPaths);

                auto alarmToneModel       = std::make_unique<bell_settings::AlarmToneModel>(this);
                auto alarmVolumeModel     = std::make_unique<bell_settings::AlarmVolumeModel>(*audioModel);
                auto alarmFadeOnOffModel  = std::make_unique<bell_settings::AlarmFadeOnOffModel>(this);
                auto alarmLightOnOffModel = std::make_unique<bell_settings::AlarmLightOnOffModel>(this);
                auto alarmFrontlightModel = std::make_unique<bell_settings::AlarmFrontlightModel>(this);
                auto alarmSettingsModel =
                    std::make_unique<bell_settings::AlarmSettingsModel>(std::move(alarmToneModel),
                                                                        std::move(alarmVolumeModel),
                                                                        std::move(alarmFadeOnOffModel),
                                                                        std::move(alarmLightOnOffModel),
                                                                        std::move(alarmFrontlightModel));

                auto frontlightModel = std::make_unique<bell_settings::FrontlightModel>(app);
                auto provider = std::make_unique<bell_settings::AlarmSettingsListItemProvider>(*alarmSettingsModel,
                                                                                               std::move(songsModel));
                auto audioErrorModel = std::make_unique<bell_settings::AudioErrorModel>();
                auto presenter       = std::make_unique<bell_settings::AlarmSettingsPresenter>(app,
                                                                                         std::move(provider),
                                                                                         std::move(alarmSettingsModel),
                                                                                         *audioModel,
                                                                                         std::move(frontlightModel),
                                                                                         std::move(audioErrorModel));
                return std::make_unique<gui::BellSettingsAlarmSettingsWindow>(app, std::move(presenter));
            });

        windowsFactory.attach(
            gui::AboutYourBellWindow::name, [](ApplicationCommon *app, [[maybe_unused]] const std::string &name) {
                auto aboutYourBellModel = std::make_shared<bell_settings::AboutYourBellModel>(app);
                auto aboutYourBellPresenter =
                    std::make_unique<bell_settings::AboutYourBellWindowPresenter>(std::move(aboutYourBellModel));
                return std::make_unique<gui::AboutYourBellWindow>(app, std::move(aboutYourBellPresenter));
            });

        windowsFactory.attach(gui::window::name::bellSettingsShortcuts,
                              [&](ApplicationCommon *app, const std::string &name) {
                                  auto presenter = std::make_unique<bell_settings::ShortcutsWindowPresenter>(this);
                                  return std::make_unique<gui::ShortcutsWindow>(app, std::move(presenter), name);
                              });

        windowsFactory.attach(
            gui::window::name::bellSettingsUpdateInstruction, [&](ApplicationCommon *app, const std::string &name) {
                auto presenter = std::make_unique<bell_settings::UpdateInstructionWindowPresenter>(this);
                return std::make_unique<gui::UpdateInstructionWindow>(app, std::move(presenter), name);
            });

        attachPopups({gui::popup::ID::AlarmActivated,
                      gui::popup::ID::AlarmDeactivated,
                      gui::popup::ID::PowerOff,
                      gui::popup::ID::Reboot,
                      gui::popup::ID::BedtimeNotification,
                      gui::popup::ID::ChargingNotification,
                      gui::popup::ID::ChargingDoneNotification,
                      gui::popup::ID::AudioError});
    }

    sys::MessagePointer ApplicationBellSettings::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {
        auto retMsg = Application::DataReceivedHandler(msgl);
        if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
            return retMsg;
        }

        // handle database response
        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return sys::msgHandled();
        }
        return handleAsyncResponse(resp);
    }

    void ApplicationBellSettings::onStop()
    {
        Application::onStop();
        audioModel->stopPlayedByThis({});
    }
} // namespace app
