// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace bsp
{
    enum class NotificationSource : uint8_t
    {
        leftSideKeyPress = 1,
        leftSideKeyRelease,
        rightSideKeyPress,
        rightSideKeyRelease,
        lightCenterKeyPress,
        lightCenterKeyRelease,
        latchKeyPress,
        latchKeyRelease,
        rotaryEncoder,
        Invalid = 0xFF
    };
} // namespace bsp
