// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-desktop/DesktopEvent.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>

namespace sdesktop::bluetooth
{
    class BluetoothEvent : public Event
    {
      public:
        BluetoothEvent();
    };

    class GetAvailableDevicesEvent : public BluetoothEvent
    {
      public:
        GetAvailableDevicesEvent() = default;
    };

} // namespace sdesktop::bluetooth
