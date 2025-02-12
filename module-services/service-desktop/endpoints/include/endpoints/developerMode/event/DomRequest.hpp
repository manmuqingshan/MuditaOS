// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <service-desktop/DeveloperModeMessage.hpp>

namespace sdesktop::developerMode
{
    class DomRequestEvent : public sdesktop::Event
    {
      public:
        explicit DomRequestEvent(sdesktop::Event &);
        DomRequestEvent() = delete;
        void setJson(json11::Json json);
    };
} // namespace sdesktop::developerMode
