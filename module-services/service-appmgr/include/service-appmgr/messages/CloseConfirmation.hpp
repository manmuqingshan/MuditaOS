// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BaseMessage.hpp"

namespace app::manager
{
    class CloseConfirmation : public BaseMessage
    {
      public:
        explicit CloseConfirmation(const ApplicationName &sender);
    };
} // namespace app::manager
