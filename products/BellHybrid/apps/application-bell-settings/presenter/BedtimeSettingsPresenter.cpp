// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedtimeSettingsPresenter.hpp"

namespace app::bell_settings
{
    BedtimeSettingsPresenter::BedtimeSettingsPresenter(std::shared_ptr<BedtimeSettingsListItemProvider> provider,
                                                       std::shared_ptr<AbstractBedtimeModel> model,
                                                       std::unique_ptr<AbstractAudioModel> audioModel,
                                                       std::unique_ptr<AbstractSoundsRepository> soundsRepository)
        : provider(std::move(provider)),
          model(std::move(model)), audioModel{std::move(audioModel)}, soundsRepository{std::move(soundsRepository)}
    {
        auto playResponseCb = [this](audio::RetCode retCode, audio::Token token) {
            if (retCode != audio::RetCode::Success || !token.IsValid()) {
                LOG_ERROR("Audio preview callback failed with retcode = %s. Token validity: %d",
                          str(retCode).c_str(),
                          token.IsValid());
                return;
            }
            this->currentToken = token;
        };

        auto playSound = [this, playResponseCb](const UTF8 &val) {
            currentSoundPath = val;
            this->audioModel->play(this->soundsRepository->titleToPath(currentSoundPath).value_or(""),
                                   playResponseCb,
                                   AbstractAudioModel::PlaybackType::Bedtime);
        };

        this->provider->onExit = [this]() { getView()->exit(); };

        this->provider->onToneEnter  = playSound;
        this->provider->onToneExit   = [this](const auto &) { stopSound(); };
        this->provider->onToneChange = playSound;

        this->provider->onVolumeEnter  = playSound;
        this->provider->onVolumeExit   = [this](const auto &) { this->audioModel->stop(currentToken, nullptr); };
        this->provider->onVolumeChange = [this, playSound](const auto &val) {
            this->audioModel->setVolume(val, AbstractAudioModel::PlaybackType::Bedtime);
            playSound(currentSoundPath);
        };
    }

    auto BedtimeSettingsPresenter::saveData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->getValue();
        }
    }

    auto BedtimeSettingsPresenter::loadData() -> void
    {
        for (const auto &item : provider->getListItems()) {
            item->setValue();
        }
    }

    auto BedtimeSettingsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return provider;
    }

    void BedtimeSettingsPresenter::eraseProviderData()
    {
        provider->clearData();
    }

    void BedtimeSettingsPresenter::stopSound()
    {
        this->audioModel->stop(currentToken, nullptr);
    }
} // namespace app::bell_settings