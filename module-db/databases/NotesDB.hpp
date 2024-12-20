// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "module-db/Tables/NotesTable.hpp"

class NotesDB : public Database
{
  public:
    NotesDB(const char *name);
    ~NotesDB() = default;

    NotesTable notes;
};
