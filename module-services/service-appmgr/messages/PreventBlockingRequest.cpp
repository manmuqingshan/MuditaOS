// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PreventBlockingRequest.hpp"

namespace app::manager
{
    PreventBlockingRequest::PreventBlockingRequest(const ApplicationName &senderName)
        : BaseMessage(MessageType::APMPreventBlocking, senderName)
    {}
} // namespace app::manager
