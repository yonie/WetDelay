//------------------------------------------------------------------------
// monobus.h - runs a stereo processor on mono buses.
//
// Every WET plug-in is written for two channels. A host can also ask for a
// mono input, a mono output, or both (Logic's "Mono" and "Mono -> Stereo"
// versions, a mono track in Cubase or Studio One). The rules, decided
// 2026-09-26:
//
//   - all four layouts are accepted: mono->mono, mono->stereo,
//     stereo->stereo, stereo->mono
//   - a mono input is copied to both sides, so the processor sees the same
//     signal on left and right and both input meters show the same level
//   - a mono output is the average of the processed left and right
//
// The copy goes into a scratch buffer rather than pointing both sides at the
// host's buffer, because a host may process in place: writing the left
// output would then overwrite the input the right side still has to read.
//
// Canonical copy in the WET hub, tools/monobus.h; copied verbatim into each
// plug-in's source folder.
//------------------------------------------------------------------------
#pragma once

#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/vstspeaker.h"

#include <cstring>
#include <vector>

namespace Wet {

class MonoBus
{
public:
    // Mono or stereo on either side; nothing else.
    static bool accepts(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
                        Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts)
    {
        using namespace Steinberg::Vst;
        auto ok = [](SpeakerArrangement a) { return a == SpeakerArr::kMono || a == SpeakerArr::kStereo; };
        return numIns == 1 && numOuts == 1 && ok(inputs[0]) && ok(outputs[0]);
    }

    // Call from setupProcessing, with the host's largest block.
    void prepare(Steinberg::int32 maxSamples)
    {
        const size_t n = maxSamples > 0 ? static_cast<size_t>(maxSamples) : 4096;
        right.assign(n, 0.0f);
        outRight.assign(n, 0.0f);
    }

    // Stereo pointers for this block. False when a bus has no channels.
    bool begin(Steinberg::Vst::AudioBusBuffers& in, Steinberg::Vst::AudioBusBuffers& out,
               Steinberg::int32 numSamples,
               float*& inL, float*& inR, float*& outL, float*& outR)
    {
        if (in.numChannels < 1 || out.numChannels < 1 || numSamples <= 0)
            return false;
        if (static_cast<size_t>(numSamples) > right.size())
            prepare(numSamples);   // a host that ignores maxSamplesPerBlock

        inL = in.channelBuffers32[0];
        if (in.numChannels >= 2)
            inR = in.channelBuffers32[1];
        else
        {
            std::memcpy(right.data(), inL, sizeof(float) * numSamples);
            inR = right.data();
        }

        outL = out.channelBuffers32[0];
        outR = out.numChannels >= 2 ? out.channelBuffers32[1] : outRight.data();
        return true;
    }

    // After processing: fold to the mono output if there is one.
    void end(Steinberg::Vst::AudioBusBuffers& out, Steinberg::int32 numSamples,
             float* outL, const float* outR)
    {
        if (out.numChannels == 1)
            for (Steinberg::int32 i = 0; i < numSamples; ++i)
                outL[i] = 0.5f * (outL[i] + outR[i]);
    }

private:
    std::vector<float> right;      // mono input, copied for the right side
    std::vector<float> outRight;   // right output when the host has only one
};

} // namespace Wet
