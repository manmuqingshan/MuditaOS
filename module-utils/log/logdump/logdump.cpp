// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <logdump/logdump.h>
#include <Logger.hpp>

using namespace Log;

int diagnosticDumpLogs()
{
    return Logger::get().diagnosticDump();
}

int shutdownFlushLogs()
{
    return Logger::get().flushLogs();
}
