﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include <endpoints/deviceInfo/DeviceInfoEndpointCommon.hpp>

namespace sdesktop::endpoints
{
    class DeviceInfoEndpoint : public DeviceInfoEndpointCommon
    {
      public:
        explicit DeviceInfoEndpoint(sys::Service *ownerServicePtr) : DeviceInfoEndpointCommon(ownerServicePtr)
        {}

        auto getSerialNumber() -> std::string;
        auto getCaseColour() -> std::string;
        auto getOnboardingState() -> OnboardingState;
        auto getDeviceInfo(Context &context) -> http::Code override;
    };
} // namespace sdesktop::endpoints
