﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>
#include <common/models/BatteryModel.hpp>
#include <common/models/LowBatteryInfoModel.hpp>

namespace app::meditation::models
{
    class ChimeInterval;
    class ChimeVolume;
    class StartDelay;
    class Statistics;
} // namespace app::meditation::models

namespace app
{
    inline constexpr auto applicationMeditationTimerName = "ApplicationMeditationTimer";

    class MeditationTimer : public Application
    {
      public:
        explicit MeditationTimer(std::string name                    = applicationMeditationTimerName,
                                 std::string parent                  = "",
                                 StatusIndicators statusIndicators   = StatusIndicators{},
                                 StartInBackground startInBackground = {false},
                                 std::uint32_t stackDepth            = 1024 * 8);
        ~MeditationTimer();

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

      private:
        std::unique_ptr<app::meditation::models::ChimeInterval> chimeIntervalModel;
        std::unique_ptr<app::meditation::models::ChimeVolume> chimeVolumeModel;
        std::unique_ptr<app::meditation::models::StartDelay> startDelayModel;
        std::unique_ptr<AbstractAudioModel> audioModel;
        std::unique_ptr<app::meditation::models::Statistics> statisticsModel;
        std::shared_ptr<sys::CpuSentinel> cpuSentinel;
        std::unique_ptr<AbstractBatteryModel> batteryModel;
        std::unique_ptr<AbstractLowBatteryInfoModel> lowBatteryInfoModel;
    };

    template <>
    struct ManifestTraits<MeditationTimer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
