// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "models/AudioErrorModel.hpp"
#include "models/alarm_settings/BedtimeSettingsListItemProvider.hpp"
#include <common/models/AbstractAudioModel.hpp>
#include <common/SoundsRepository.hpp>
#include <common/models/AbstractBedtimeModel.hpp>
#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
} // namespace gui

namespace app::bell_settings
{
    class BedtimeSettingsWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept                 = default;
            virtual auto exit() -> void              = 0;
            virtual auto deepRefresh() -> void       = 0;
            virtual auto handleError() -> void       = 0;
            virtual auto handleDeletedFile() -> void = 0;
        };

        class Presenter : public BasePresenter<View>
        {
          public:
            virtual ~Presenter() noexcept                                                   = default;
            virtual auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> = 0;
            virtual auto saveData() -> void                                                 = 0;
            virtual auto loadData() -> void                                                 = 0;
            virtual auto eraseProviderData() -> void                                        = 0;
            virtual auto exitWithoutSave() -> void                                          = 0;
        };
    };

    class SettingsPresenter : public BedtimeSettingsWindowContract::Presenter
    {
      public:
        SettingsPresenter(ApplicationCommon *app,
                          std::unique_ptr<BedtimeSettingsListItemProvider> &&provider,
                          std::shared_ptr<AbstractBedtimeModel> model,
                          AbstractAudioModel &audioModel,
                          std::unique_ptr<AudioErrorModel> &&audioErrorModel);

        auto getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider> override;
        auto saveData() -> void override;
        auto loadData() -> void override;
        auto eraseProviderData() -> void override;
        auto exitWithoutSave() -> void override;

      private:
        auto stopSound() -> void;
        auto handleAudioError(const UTF8 &path, gui::AudioErrorType errorType) -> void;
        auto showAudioError(gui::AudioErrorType errorType) const -> void;
        auto validatePath(const UTF8 &path) const -> bool;

        ApplicationCommon *app;
        std::shared_ptr<BedtimeSettingsListItemProvider> provider;
        std::shared_ptr<AbstractBedtimeModel> model;
        AbstractAudioModel &audioModel;
        std::unique_ptr<AudioErrorModel> audioErrorModel;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
