// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "Common/Query.hpp"
#include <string>

namespace db::query::alarms
{

    class Remove : public Query
    {
      public:
        explicit Remove(const uint32_t &id);
        [[nodiscard]] auto debugInfo() const -> std::string override;

        const uint32_t id;
    };

    class RemoveResult : public QueryResult
    {
        bool ret = true;

      public:
        explicit RemoveResult(const bool &ret);
        [[nodiscard]] auto succeed() const -> bool;
        [[nodiscard]] auto debugInfo() const -> std::string override;
    };

} // namespace db::query::alarms