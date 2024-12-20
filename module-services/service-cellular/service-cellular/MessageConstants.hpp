// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

namespace msgConstants
{
    constexpr uint32_t singleMessageMaxLen  = 67;
    constexpr uint32_t maxConcatenatedCount = 7;
    constexpr uint32_t maxConcatenatedLen   = singleMessageMaxLen * maxConcatenatedCount;
} // namespace msgConstants