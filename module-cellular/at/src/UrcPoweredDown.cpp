// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <UrcPoweredDown.hpp>
#include <Utils.hpp>

using namespace at::urc;

bool PoweredDown::isValid() const noexcept
{
    return isNormalPowerDown() || isImmediatePowerDown();
}

bool PoweredDown::isNormalPowerDown() const noexcept
{
    return urcBody.find(PoweredDown::head_normal) != std::string::npos;
}

bool PoweredDown::isImmediatePowerDown() const noexcept
{
    return urcBody.find(PoweredDown::head_immediate) != std::string::npos;
}
