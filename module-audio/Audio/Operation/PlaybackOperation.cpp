// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PlaybackOperation.hpp"

#include "Audio/decoder/Decoder.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/StreamFactory.hpp"

#include "Audio/AudioCommon.hpp"

#include <log/log.hpp>

namespace audio
{
    using namespace AudioServiceMessage;

    PlaybackOperation::PlaybackOperation(const std::string &filePath,
                                         const PlaybackType &playbackType,
                                         const PlaybackMode &playbackMode,
                                         Callback callback)
        : Operation(std::move(callback), playbackType), playbackMode(playbackMode), dec(nullptr)
    {
        // order defines priority
        AddProfile(Profile::Type::PlaybackHeadphones, playbackType, false);
        AddProfile(Profile::Type::PlaybackBluetoothA2DP, playbackType, false);
        AddProfile(Profile::Type::PlaybackLoudspeaker, playbackType, true);

        endOfFileCallback = [this]() {
            if (this->playbackMode == PlaybackMode::Single) {
                state          = State::Idle;
                const auto msg = AudioServiceMessage::EndOfFile(operationToken);
                serviceCallback(&msg);
            }
            else {
                dec->rewind();
            }
        };

        fileDeletedCallback = [this]() {
            state          = State::Idle;
            const auto msg = AudioServiceMessage::FileDeleted(operationToken);
            serviceCallback(&msg);
        };

        dec = Decoder::Create(filePath);
        if (dec == nullptr) {
            throw AudioInitException("Error during initializing decoder", RetCode::FileDoesntExist);
        }
        auto format = dec->getSourceFormat();
        LOG_DEBUG("Source format: %s", format.toString().c_str());

        auto retCode = SwitchToPriorityProfile(playbackType);
        if (retCode != RetCode::Success) {
            throw AudioInitException("Failed to switch audio profile", retCode);
        }
    }

    RetCode PlaybackOperation::Start(Token token)
    {
        if (state == State::Active || (state == State::Paused && outputConnection != nullptr)) {
            return RetCode::InvokedInIncorrectState;
        }

        // create stream
        StreamFactory streamFactory(playbackTimeConstraint);
        try {
            dataStreamOut = streamFactory.makeStream(*dec, *audioDevice, currentProfile->getAudioFormat());
        }
        catch (std::invalid_argument &e) {
            LOG_FATAL("Cannot create audio stream: %s", e.what());
            return RetCode::Failed;
        }

        // create audio connection
        outputConnection = std::make_unique<StreamConnection>(dec.get(), audioDevice.get(), dataStreamOut.get());

        // decoder worker soft start - must be called after connection setup
        dec->startDecodingWorker(endOfFileCallback, fileDeletedCallback);

        // start output device and enable audio connection
        auto ret = audioDevice->Start();
        outputConnection->enable();

        // update state and token
        state          = State::Active;
        operationToken = token;

        return GetDeviceError(ret);
    }

    RetCode PlaybackOperation::Stop()
    {
        state = State::Idle;
        if (!audioDevice) {
            return RetCode::DeviceFailure;
        }

        // stop playback by destroying audio connection
        outputConnection.reset();
        dec->stopDecodingWorker();
        dataStreamOut.reset();

        return GetDeviceError(audioDevice->Stop());
    }

    RetCode PlaybackOperation::Pause()
    {
        if (state == State::Paused || state == State::Idle || outputConnection == nullptr) {
            return RetCode::InvokedInIncorrectState;
        }
        const auto retCode = GetDeviceError(audioDevice->Pause());
        if (retCode == RetCode::Success) {
            state = State::Paused;
            outputConnection->disable();
        }
        return retCode;
    }

    RetCode PlaybackOperation::Resume()
    {
        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        if (outputConnection == nullptr) {
            Start(operationToken);
        }

        state = State::Active;
        outputConnection->enable();
        return GetDeviceError(audioDevice->Resume());
    }

    RetCode PlaybackOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->setOutputVolume(vol);
        return GetDeviceError(ret);
    }

    RetCode PlaybackOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->setInputGain(gain);
        return GetDeviceError(ret);
    }

    Position PlaybackOperation::GetPosition()
    {
        return dec->getCurrentPosition();
    }

    RetCode PlaybackOperation::SwitchToPriorityProfile(PlaybackType playbackType)
    {
        for (const auto &p : supportedProfiles) {
            const auto profileType = p.profile->GetType();
            if (profileType == Profile::Type::PlaybackBluetoothA2DP && playbackType == PlaybackType::CallRingtone) {
                continue;
            }
            if (p.isAvailable) {
                return SwitchProfile(profileType);
            }
        }
        return RetCode::ProfileNotSet;
    }

    RetCode PlaybackOperation::SendEvent(std::shared_ptr<Event> evt)
    {
        const auto isAvailable = evt->getDeviceState() == Event::DeviceState::Connected;
        switch (evt->getType()) {
        case EventType::JackState:
            SetProfileAvailability({Profile::Type::PlaybackHeadphones}, isAvailable);
            Operation::SwitchToPriorityProfile();
            break;
        case EventType::BluetoothA2DPDeviceState:
            SetProfileAvailability({Profile::Type::PlaybackBluetoothA2DP}, isAvailable);
            Operation::SwitchToPriorityProfile();
            break;
        default:
            return RetCode::UnsupportedEvent;
        }

        return RetCode::Success;
    }

    RetCode PlaybackOperation::SwitchProfile(const Profile::Type type)
    {
        auto newProfile = GetProfile(type);
        if (newProfile == nullptr) {
            LOG_ERROR("Unsupported profile");
            return RetCode::UnsupportedProfile;
        }

        if (currentProfile && currentProfile->GetType() == newProfile->GetType()) {
            return RetCode::Success;
        }

        // adjust new profile with information from file's tags
        newProfile->SetSampleRate(dec->getSourceFormat().getSampleRate());
        newProfile->SetInOutFlags(static_cast<std::uint32_t>(codec::Flags::OutputStereo));

        /// profile change - (re)create output device; stop audio first by
        /// killing audio connection
        outputConnection.reset();
        dec->stopDecodingWorker();
        audioDevice.reset();
        dataStreamOut.reset();
        audioDevice = CreateDevice(*newProfile);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

        // check if audio device supports Decoder's profile
        if (auto format = dec->getSourceFormat(); !audioDevice->isFormatSupportedBySink(format)) {
            LOG_ERROR("Format unsupported by the audio device: %s", format.toString().c_str());
            return RetCode::InvalidFormat;
        }

        // store profile
        currentProfile = newProfile;

        if (state == State::Active) {
            // playback in progress, restart
            state = State::Idle;
            Start(operationToken);
        }

        return RetCode::Success;
    }

    PlaybackOperation::~PlaybackOperation()
    {
        Stop();
    }
} // namespace audio
