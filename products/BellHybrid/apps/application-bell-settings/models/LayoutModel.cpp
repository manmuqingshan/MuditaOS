// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <models/LayoutModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <db/SystemSettings.hpp>

namespace app::bell_settings
{

    LayoutModel::LayoutModel(ApplicationCommon *app)
    {
        settings.init(service::ServiceProxy{app->weak_from_this()});
    }

    std::string LayoutModel::getValue() const
    {
        return settings.getValue(bell::settings::Layout::defaultLayout, settings::SettingsScope::Global);
    }
} // namespace app::bell_settings