﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Audio/AudioCommon.hpp>
#include <MessageType.hpp>
#include <Service/Message.hpp>

#include <memory>
#include <variant>

namespace service
{
    inline constexpr auto audioServiceName = "ServiceAudio";

    class AudioMessage : public sys::DataMessage
    {
      public:
        AudioMessage() : sys::DataMessage(MessageType::AudioMessage)
        {}
    };

    class AudioResponseMessage : public sys::ResponseMessage
    {
      public:
        explicit AudioResponseMessage(audio::RetCode retCode = audio::RetCode::Success, const std::string &val = {})
            : sys::ResponseMessage(), retCode(retCode), val(val)
        {}

        const audio::RetCode retCode = audio::RetCode::Success;
        std::string val;
    };

    class AudioNotificationMessage : public AudioMessage
    {
      public:
        explicit AudioNotificationMessage(audio::Token token) : token{token}
        {}

        const audio::Token token;
    };

    class AudioStopNotification : public AudioNotificationMessage
    {
      public:
        explicit AudioStopNotification(audio::Token token) : AudioNotificationMessage{token}
        {}
    };

    class AudioEOFNotification : public AudioNotificationMessage
    {
      public:
        explicit AudioEOFNotification(audio::Token token) : AudioNotificationMessage{token}
        {}
    };

    class AudioFileDeletedNotification : public AudioNotificationMessage
    {
      public:
        explicit AudioFileDeletedNotification(audio::Token token) : AudioNotificationMessage{token}
        {}
    };

    class AudioSettingsMessage : public AudioMessage
    {
      public:
        explicit AudioSettingsMessage(const audio::PlaybackType &playbackType, const std::string &value = {})
            : AudioMessage{}, playbackType{playbackType}, value{value}
        {}

        audio::PlaybackType playbackType = audio::PlaybackType::None;
        std::string value{};
    };

    class AudioGetVolume : public AudioSettingsMessage
    {
      public:
        explicit AudioGetVolume(const audio::PlaybackType &playbackType) : AudioSettingsMessage{playbackType}
        {}
    };

    class AudioSetVolume : public AudioSettingsMessage
    {
      public:
        AudioSetVolume(const audio::PlaybackType &playbackType,
                       const audio::VolumeUpdateType updateType,
                       const std::string &val)
            : AudioSettingsMessage{playbackType, val}, updateType{updateType}
        {}

        audio::VolumeUpdateType updateType;
    };

    class AudioStopRequest : public AudioMessage
    {
      public:
        explicit AudioStopRequest(const std::vector<audio::PlaybackType> &stopVec = {}) : stopVec(stopVec)
        {}

        explicit AudioStopRequest(const audio::Token &token) : token(token)
        {}

        const std::vector<audio::PlaybackType> stopVec;
        const audio::Token token;
    };

    class AudioStopResponse : public AudioResponseMessage
    {
      public:
        AudioStopResponse(audio::RetCode retCode, const audio::Token &token)
            : AudioResponseMessage(retCode), token(token)
        {}

        const audio::Token token;
    };

    class AudioStartPlaybackRequest : public AudioMessage
    {
      public:
        AudioStartPlaybackRequest(const std::string &fileName,
                                  const audio::PlaybackType &playbackType,
                                  const audio::PlaybackMode &playbackMode     = audio::PlaybackMode::Single,
                                  std::optional<audio::FadeParams> fadeParams = std::nullopt)
            : AudioMessage(), fileName(fileName), playbackType(playbackType), playbackMode(playbackMode),
              fadeParams(fadeParams)
        {}

        const std::string fileName;
        const audio::PlaybackType playbackType;
        const audio::PlaybackMode playbackMode;
        const std::optional<audio::FadeParams> fadeParams;
    };

    class AudioStartPlaybackResponse : public AudioResponseMessage
    {
      public:
        AudioStartPlaybackResponse(audio::RetCode retCode, const audio::Token &token)
            : AudioResponseMessage(retCode), token(token)
        {}

        const audio::Token token;
    };

    class AudioPauseRequest : public AudioMessage
    {
      public:
        AudioPauseRequest() : AudioMessage()
        {}
    };

    class AudioResumeRequest : public AudioMessage
    {
      public:
        AudioResumeRequest() : AudioMessage()
        {}
    };
} // namespace service
