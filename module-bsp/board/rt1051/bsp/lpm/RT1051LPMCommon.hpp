// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "bsp/lpm/bsp_lpm.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "CpuFreqLPM.hpp"

namespace bsp
{
    class RT1051LPMCommon : public LowPowerMode
    {
      public:
        RT1051LPMCommon();
        int32_t Reboot(RebootType reason) override final;
        void SetCpuFrequency(CpuFrequencyHz freq) final;
        void SetHighestCoreVoltage() final;
        [[nodiscard]] uint32_t GetCpuFrequency() const noexcept final;
        void SwitchOscillatorSource(OscillatorSource source) final;
        void SetBootSuccess() override;

        void DisconnectInternalLoadResistor() final;
        void ConnectInternalLoadResistor() final;

        void SwitchToRegularModeLDO() final;
        void SwitchToLowPowerModeLDO() final;

      private:
        std::unique_ptr<bsp::CpuFreqLPM> CpuFreq;
    };

} // namespace bsp