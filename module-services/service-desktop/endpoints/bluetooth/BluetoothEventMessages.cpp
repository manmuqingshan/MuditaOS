// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <endpoints/bluetooth/BluetoothEventMessages.hpp>

namespace sdesktop::bluetooth
{
    BluetoothEvent::BluetoothEvent()
    {
        context.setResponseStatus(endpoints::http::Code::OK);
        context.setEndpoint(endpoints::EndpointType::Bluetooth);
    }
} // namespace sdesktop::bluetooth
