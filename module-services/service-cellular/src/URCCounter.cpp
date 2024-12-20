// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "URCCounter.hpp"

namespace cellular::service
{

    void URCCounter::count()
    {
        urcOccurences++;
    }
    void URCCounter::clearCounter()
    {
        urcOccurences = 0;
    }
    auto URCCounter::getCounter() -> uint32_t
    {
        return urcOccurences;
    }
} // namespace cellular::service
