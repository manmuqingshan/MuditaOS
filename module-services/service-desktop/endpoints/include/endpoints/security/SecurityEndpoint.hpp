﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "SecurityEndpointHelper.hpp"

namespace sdesktop::endpoints
{

    class SecurityEndpoint : public Endpoint
    {
        const std::unique_ptr<SecurityEndpointHelper> helper;

      public:
        explicit SecurityEndpoint(sys::Service *ownerServicePtr)
            : Endpoint(ownerServicePtr), helper(std::make_unique<SecurityEndpointHelper>(ownerServicePtr))
        {
            debugName = "SecurityEndpoint";
        }

        auto handle(Context &context) -> void override;
    };

} // namespace sdesktop::endpoints
