//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "delaybuffer.h"
#include <algorithm>
#include <cmath>

namespace Yonie {

//------------------------------------------------------------------------
DelayBuffer::DelayBuffer()
: writePos(0)
, maxSamples(0)
, hostSampleRate(44100.0)
, rng(std::random_device{}())
, ditherDist(-1.0f, 1.0f)  // TPDF dither: uniform -1 to +1
{
}

//------------------------------------------------------------------------
DelayBuffer::~DelayBuffer()
{
}

//------------------------------------------------------------------------
void DelayBuffer::prepare(double sampleRate, int maxDelayMs)
{
    hostSampleRate = sampleRate;
    
    // Calculate max samples at INTERNAL 24 kHz rate
    maxSamples = static_cast<int>(maxDelayMs * INTERNAL_SAMPLE_RATE / 1000.0);
    
    // Allocate main delay buffers (at internal 24 kHz rate)
    bufferL.resize(maxSamples, 0.0f);
    bufferR.resize(maxSamples, 0.0f);
    
    writePos = 0;
    
    // Calculate max block size at internal rate
    // Assuming max 4096 samples at host rate, calculate equivalent at internal rate
    int maxInternalBlockSize = static_cast<int>(4096 * INTERNAL_SAMPLE_RATE / sampleRate) + 16;
    
    // Allocate temporary buffers for resampling
    tempDownL.resize(maxInternalBlockSize, 0.0f);
    tempDownR.resize(maxInternalBlockSize, 0.0f);
    tempDelayedL.resize(maxInternalBlockSize, 0.0f);
    tempDelayedR.resize(maxInternalBlockSize, 0.0f);
    
    // Anti-aliasing filter before downsampling (at HOST rate)
    // Cut at 10 kHz to prevent aliasing when downsampling to 24 kHz
    antiAliasL.setCoefficients(sampleRate, ANTI_ALIAS_FREQ, OnePoleFilter::Type::LowPass);
    antiAliasR.setCoefficients(sampleRate, ANTI_ALIAS_FREQ, OnePoleFilter::Type::LowPass);
    
    // Reconstruction filter after upsampling (at HOST rate)
    // Smooths out the stepped output
    reconstructL.setCoefficients(sampleRate, ANTI_ALIAS_FREQ, OnePoleFilter::Type::LowPass);
    reconstructR.setCoefficients(sampleRate, ANTI_ALIAS_FREQ, OnePoleFilter::Type::LowPass);
    
    // Initialize character filters (at INTERNAL 24 kHz rate)
    // High-pass filter: 1st-order (6 dB/oct) @ 80 Hz
    highPassL.setCoefficients(INTERNAL_SAMPLE_RATE, HIGH_PASS_FREQ, OnePoleFilter::Type::HighPass);
    highPassR.setCoefficients(INTERNAL_SAMPLE_RATE, HIGH_PASS_FREQ, OnePoleFilter::Type::HighPass);
    
    // Low-pass filter: 1st-order (6 dB/oct) @ 9 kHz
    lowPassL.setCoefficients(INTERNAL_SAMPLE_RATE, LOW_PASS_FREQ, OnePoleFilter::Type::LowPass);
    lowPassR.setCoefficients(INTERNAL_SAMPLE_RATE, LOW_PASS_FREQ, OnePoleFilter::Type::LowPass);
    
    // Reset all filter states
    antiAliasL.reset();
    antiAliasR.reset();
    reconstructL.reset();
    reconstructR.reset();
    highPassL.reset();
    highPassR.reset();
    lowPassL.reset();
    lowPassR.reset();
    
    // Reset resamplers
    downsamplerL.reset();
    downsamplerR.reset();
    upsamplerL.reset();
    upsamplerR.reset();
}

//------------------------------------------------------------------------
void DelayBuffer::processStereo(float* leftIn, float* leftOut,
                                float* rightIn, float* rightOut,
                                int numSamples, int delayMs)
{
    if (bufferL.empty() || bufferR.empty())
        return;
    
    // Calculate delay in samples at INTERNAL rate
    int delaySamples = msToSamples(delayMs);
    delaySamples = std::max(1, std::min(delaySamples, maxSamples - 1));
    
    // Step 1: Anti-alias filter the input (at host rate)
    std::vector<float> filteredInL(numSamples);
    std::vector<float> filteredInR(numSamples);
    for (int i = 0; i < numSamples; ++i)
    {
        filteredInL[i] = antiAliasL.process(leftIn[i]);
        filteredInR[i] = antiAliasR.process(rightIn[i]);
    }
    
    // Step 2: Downsample to internal 24 kHz rate
    int internalSamples = static_cast<int>(numSamples * INTERNAL_SAMPLE_RATE / hostSampleRate) + 1;
    internalSamples = std::min(internalSamples, static_cast<int>(tempDownL.size()));
    
    int actualDownL = downsamplerL.downsample(filteredInL.data(), numSamples,
                                               tempDownL.data(), internalSamples,
                                               hostSampleRate, INTERNAL_SAMPLE_RATE);
    int actualDownR = downsamplerR.downsample(filteredInR.data(), numSamples,
                                               tempDownR.data(), internalSamples,
                                               hostSampleRate, INTERNAL_SAMPLE_RATE);
    
    int actualInternal = std::min(actualDownL, actualDownR);
    
    // Step 3: Process through delay line at internal rate
    for (int i = 0; i < actualInternal; ++i)
    {
        processInternalSample(tempDownL[i], tempDownR[i],
                              tempDelayedL[i], tempDelayedR[i],
                              delaySamples);
    }
    
    // Step 4: Upsample back to host rate
    upsamplerL.upsample(tempDelayedL.data(), actualInternal,
                        leftOut, numSamples,
                        INTERNAL_SAMPLE_RATE, hostSampleRate);
    upsamplerR.upsample(tempDelayedR.data(), actualInternal,
                        rightOut, numSamples,
                        INTERNAL_SAMPLE_RATE, hostSampleRate);
    
    // Step 5: Reconstruction filter (smooths stepped output)
    for (int i = 0; i < numSamples; ++i)
    {
        leftOut[i] = reconstructL.process(leftOut[i]);
        rightOut[i] = reconstructR.process(rightOut[i]);
    }
}

//------------------------------------------------------------------------
void DelayBuffer::processInternalSample(float inputL, float inputR,
                                        float& outputL, float& outputR,
                                        int delaySamples)
{
    // Calculate read position
    int readPos = writePos - delaySamples;
    if (readPos < 0)
        readPos += maxSamples;
    
    // Write input to buffer
    bufferL[writePos] = inputL;
    bufferR[writePos] = inputR;
    
    // Read delayed output
    float delayedL = bufferL[readPos];
    float delayedR = bufferR[readPos];
    
    // Apply channel crosstalk (authentic 80s analog bleed between L/R)
    // Small amount of each channel bleeds into the other
    float crosstalkedL = delayedL + CROSSTALK_AMOUNT * delayedR;
    float crosstalkedR = delayedR + CROSSTALK_AMOUNT * delayedL;
    delayedL = crosstalkedL;
    delayedR = crosstalkedR;
    
    // Apply character filters (at internal 24 kHz rate)
    // High-pass (80 Hz) - removes low rumble
    delayedL = highPassL.process(delayedL);
    delayedR = highPassR.process(delayedR);
    
    // Low-pass (9 kHz) - warm high-end rolloff
    delayedL = lowPassL.process(delayedL);
    delayedR = lowPassR.process(delayedR);
    
    // 12-bit quantization with light dither
    // Dither reduces harsh quantization artifacts for smoother sound
    constexpr float BIT_DEPTH_LEVELS = 4096.0f;  // 2^12
    constexpr float DITHER_AMPLITUDE = 0.5f / BIT_DEPTH_LEVELS;  // Very light: 0.5 LSB
    
    // Add TPDF dither (triangular probability density function)
    // Two uniform randoms summed = triangular distribution
    float ditherL = (ditherDist(rng) + ditherDist(rng)) * DITHER_AMPLITUDE;
    float ditherR = (ditherDist(rng) + ditherDist(rng)) * DITHER_AMPLITUDE;
    
    delayedL = std::floor((delayedL + ditherL) * BIT_DEPTH_LEVELS + 0.5f) / BIT_DEPTH_LEVELS;
    delayedR = std::floor((delayedR + ditherR) * BIT_DEPTH_LEVELS + 0.5f) / BIT_DEPTH_LEVELS;
    
    // Fixed noise floor at -80 dBFS (simulates analog electronics/ADC noise)
    // -80 dBFS = 10^(-80/20) = 0.0001 peak amplitude
    constexpr float NOISE_FLOOR_AMPLITUDE = 0.0001f;
    
    float noiseL = ditherDist(rng) * NOISE_FLOOR_AMPLITUDE;
    float noiseR = ditherDist(rng) * NOISE_FLOOR_AMPLITUDE;
    
    outputL = delayedL + noiseL;
    outputR = delayedR + noiseR;
    
    // Advance write position
    writePos++;
    if (writePos >= maxSamples)
        writePos = 0;
}

//------------------------------------------------------------------------
void DelayBuffer::reset()
{
    std::fill(bufferL.begin(), bufferL.end(), 0.0f);
    std::fill(bufferR.begin(), bufferR.end(), 0.0f);
    writePos = 0;
    
    // Reset all filter states
    antiAliasL.reset();
    antiAliasR.reset();
    reconstructL.reset();
    reconstructR.reset();
    highPassL.reset();
    highPassR.reset();
    lowPassL.reset();
    lowPassR.reset();
    
    // Reset resamplers
    downsamplerL.reset();
    downsamplerR.reset();
    upsamplerL.reset();
    upsamplerR.reset();
}

//------------------------------------------------------------------------
int DelayBuffer::msToSamples(int ms) const
{
    // Convert to samples at INTERNAL 24 kHz rate
    return static_cast<int>(ms * INTERNAL_SAMPLE_RATE / 1000.0);
}

//------------------------------------------------------------------------
} // namespace Yonie