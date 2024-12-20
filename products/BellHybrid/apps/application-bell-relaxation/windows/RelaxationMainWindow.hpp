// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "presenter/RelaxationMainWindowPresenter.hpp"

#include <common/widgets/ListViewWithLabels.hpp>
#include <common/options/BellOptionWindow.hpp>

namespace gui
{
    class RelaxationMainWindow : public AppWindow, public app::relaxation::RelaxationMainWindowContract::View
    {
      public:
        RelaxationMainWindow(app::ApplicationCommon *app,
                             std::unique_ptr<app::relaxation::RelaxationMainWindowContract::Presenter> &&presenter);

      private:
        std::unique_ptr<app::relaxation::RelaxationMainWindowContract::Presenter> presenter;

        gui::ListViewWithLabels *songList{nullptr};

        void buildInterface() override;
        void rebuild() override;
        void updateViewState() override;
        void activate(const db::multimedia_files::MultimediaFilesRecord &selectedSound);
    };
} // namespace gui
