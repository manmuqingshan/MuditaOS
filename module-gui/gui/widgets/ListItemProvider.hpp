// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "ListItem.hpp"

namespace gui
{
    class ListViewEngine;

    enum class Order
    {
        Next,
        Previous
    };

    inline constexpr Order operator~(const Order &order)
    {
        switch (order) {
        case Order::Next:
            return Order::Previous;
        case Order::Previous:
        default:
            return Order::Next;
        }
    }

    class ListItemProvider
    {
      public:
        virtual ~ListItemProvider() = default;

        ListViewEngine *list = nullptr;

        virtual unsigned int requestRecordsCount() = 0;

        [[nodiscard]] virtual unsigned int getMinimalItemSpaceRequired() const = 0;

        virtual ListItem *getItem(Order order) = 0;

        virtual void requestRecords(std::uint32_t offset, std::uint32_t limit) = 0;
    };
} // namespace gui
