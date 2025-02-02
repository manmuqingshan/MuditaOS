// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md
#pragma once

#include "Profile.hpp"

namespace audio
{

    class ProfileRecordingHeadphones : public Profile
    {
      public:
        ProfileRecordingHeadphones(Gain gain)
            : Profile(
                  "Recording Headset",
                  Type::RecordingHeadphones,
                  audio::codec::Configuration{.sampleRate_Hz = 44100,
                                              .bitWidth      = 16,
                                              .flags         = static_cast<uint32_t>(
                                                  audio::codec::Flags::InputLeft), // microphone use left audio channel
                                              .outputVolume = 0,
                                              .inputGain    = static_cast<float>(gain),
                                              .inputPath    = audio::codec::InputPath::Headphones,
                                              .outputPath   = audio::codec::OutputPath::None},
                  AudioDevice::Type::Audiocodec)
        {}
    };

} // namespace audio
