// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "PlatformAgnosticSequences.hpp"

class AlarmActivateSequence : public AlarmChangeSequence
{
  public:
    AlarmActivateSequence() : AlarmChangeSequence(KeyMap::DeepPressUp)
    {}
};
