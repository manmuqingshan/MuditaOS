// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <memory>
#include <vector>
#include "SystemManager/CpuSentinel.hpp"
#include "SentinelView.hpp"

namespace sys
{

    using SentinelPointer = std::weak_ptr<CpuSentinel>;

    class GovernorSentinel
    {
      public:
        explicit GovernorSentinel(std::shared_ptr<CpuSentinel> newSentinel);
        [[nodiscard]] auto GetSentinel() const noexcept -> SentinelPointer;
        [[nodiscard]] auto GetRequestedFrequency() const noexcept -> bsp::CpuFrequencyMHz;
        void SetRequestedFrequency(bsp::CpuFrequencyMHz newFrequency);
        void BlockWfiMode(bool request);
        [[nodiscard]] auto IsWfiBlocked() const noexcept -> bool;

      private:
        SentinelPointer sentinelPtr;
        bsp::CpuFrequencyMHz requestedFrequency;
        bool wfiBlocked{false};
    };

    using GovernorSentinelPointer = std::unique_ptr<GovernorSentinel>;
    using GovernorSentinelsVector = std::vector<GovernorSentinelPointer>;

    /// CpuGovernor manages all sentinels in the system and has CPU frequency requests from them (e.g. eInkSentinel).
    /// It is also responsible for informing all sentinels that the CPU frequency has changed.
    class CpuGovernor
    {

      public:
        auto RegisterNewSentinel(std::shared_ptr<CpuSentinel> newSentinel) -> bool;
        auto RemoveSentinel(std::string sentinelName) -> void;
        [[nodiscard]] auto GetNumberOfRegisteredSentinels() const noexcept -> uint32_t;
        void PrintAllSentinels() const noexcept;
        void PrintActiveSentinels() const noexcept;

        void SetCpuFrequencyRequest(const std::string &sentinelName, bsp::CpuFrequencyMHz request);
        void ResetCpuFrequencyRequest(const std::string &sentinelName);
        void BlockWfiMode(const std::string &sentinelName, bool request);

        [[nodiscard]] auto GetMinimumFrequencyRequested() noexcept -> sentinel::View;
        [[nodiscard]] auto IsWfiBlocked() noexcept -> bool;
        void InformSentinelsAboutCpuFrequencyChange(bsp::CpuFrequencyMHz newFrequency) noexcept;

      private:
        /// this could be set - set is sorted :)
        GovernorSentinelsVector sentinels;
    };

} // namespace sys
