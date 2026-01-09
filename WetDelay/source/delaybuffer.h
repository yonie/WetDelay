//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include <vector>
#include <cstring>
#include <cmath>
#include <random>

namespace Yonie {

//------------------------------------------------------------------------
// OnePoleFilter - Simple 1st-order (6 dB/oct) filter
//------------------------------------------------------------------------
class OnePoleFilter
{
public:
    enum class Type { LowPass, HighPass };
    
    OnePoleFilter() : z1(0.0f), x1(0.0f), coefficient(0.0f), type(Type::LowPass) {}
    
    // Set filter parameters
    void setCoefficients(double sampleRate, double cutoffHz, Type filterType)
    {
        type = filterType;
        // Calculate coefficient for 1st-order filter
        // Using: coefficient = exp(-2π * fc / fs)
        double omega = 2.0 * 3.14159265358979323846 * cutoffHz / sampleRate;
        coefficient = static_cast<float>(std::exp(-omega));
    }
    
    // Process a single sample
    float process(float input)
    {
        float output;
        if (type == Type::LowPass)
        {
            // Low-pass: y[n] = (1-a) * x[n] + a * y[n-1]
            output = (1.0f - coefficient) * input + coefficient * z1;
        }
        else
        {
            // High-pass: y[n] = a * (y[n-1] + x[n] - x[n-1])
            output = coefficient * (z1 + input - x1);
        }
        z1 = output;
        x1 = input;
        return output;
    }
    
    // Reset filter state
    void reset()
    {
        z1 = 0.0f;
        x1 = 0.0f;
    }
    
private:
    float z1;          // Previous output (y[n-1])
    float x1;          // Previous input (x[n-1]) - used for high-pass
    float coefficient; // Filter coefficient
    Type type;         // Filter type
};

//------------------------------------------------------------------------
// Simple linear interpolation resampler
//------------------------------------------------------------------------
class LinearResampler
{
public:
    LinearResampler() : phase(0.0), lastSample(0.0f) {}
    
    void reset()
    {
        phase = 0.0;
        lastSample = 0.0f;
    }
    
    // Downsample from higher rate to lower rate
    // Returns number of output samples written
    int downsample(const float* input, int inputSamples,
                   float* output, int maxOutputSamples,
                   double inputRate, double outputRate)
    {
        double ratio = inputRate / outputRate;
        int outCount = 0;
        
        for (int i = 0; i < inputSamples && outCount < maxOutputSamples; ++i)
        {
            float currentSample = input[i];
            
            while (phase < 1.0 && outCount < maxOutputSamples)
            {
                // Linear interpolation
                float t = static_cast<float>(phase);
                output[outCount++] = lastSample + t * (currentSample - lastSample);
                phase += ratio;
            }
            
            phase -= 1.0;
            lastSample = currentSample;
        }
        
        return outCount;
    }
    
    // Upsample from lower rate to higher rate
    // Returns number of output samples written
    int upsample(const float* input, int inputSamples,
                 float* output, int outputSamples,
                 double inputRate, double outputRate)
    {
        double ratio = inputRate / outputRate;
        int inIndex = 0;
        
        for (int i = 0; i < outputSamples; ++i)
        {
            // Linear interpolation between samples
            float t = static_cast<float>(phase);
            float currentSample = (inIndex < inputSamples) ? input[inIndex] : lastSample;
            output[i] = lastSample + t * (currentSample - lastSample);
            
            phase += ratio;
            while (phase >= 1.0 && inIndex < inputSamples)
            {
                lastSample = input[inIndex++];
                phase -= 1.0;
            }
        }
        
        if (inIndex > 0 && inIndex <= inputSamples)
            lastSample = input[inIndex - 1];
            
        return outputSamples;
    }
    
private:
    double phase;
    float lastSample;
};

//------------------------------------------------------------------------
// DelayBuffer - Stereo circular delay buffer with 80s rack-style processing
//------------------------------------------------------------------------
class DelayBuffer
{
public:
    DelayBuffer();
    ~DelayBuffer();
    
    // Random number generator for dither
    std::mt19937 rng;
    std::uniform_real_distribution<float> ditherDist;
    
    // Prepare buffer for given sample rate and max delay
    void prepare(double sampleRate, int maxDelayMs);
    
    // Process a block of stereo samples with given delay time
    void processStereo(float* leftIn, float* leftOut,
                      float* rightIn, float* rightOut,
                      int numSamples, int delayMs);
    
    // Clear the buffer
    void reset();
    
private:
    // Internal 24 kHz sample rate (authentic 80s rack delay)
    static constexpr double INTERNAL_SAMPLE_RATE = 24000.0;
    
    // Main delay buffer (at internal rate)
    std::vector<float> bufferL;
    std::vector<float> bufferR;
    int writePos;
    int maxSamples;
    double hostSampleRate;
    
    // Resamplers for down/upsampling
    LinearResampler downsamplerL;
    LinearResampler downsamplerR;
    LinearResampler upsamplerL;
    LinearResampler upsamplerR;
    
    // Temporary buffers for resampling
    std::vector<float> tempDownL;
    std::vector<float> tempDownR;
    std::vector<float> tempDelayedL;
    std::vector<float> tempDelayedR;
    
    // Anti-aliasing filter for downsampling (cuts at ~10 kHz before 24 kHz nyquist)
    OnePoleFilter antiAliasL;
    OnePoleFilter antiAliasR;
    
    // Reconstruction filter for upsampling
    OnePoleFilter reconstructL;
    OnePoleFilter reconstructR;
    
    // Filters: High-pass @ 80Hz, Low-pass @ 9kHz (both 1st-order, 6 dB/oct)
    // These operate at the internal 24 kHz rate
    OnePoleFilter highPassL;
    OnePoleFilter highPassR;
    OnePoleFilter lowPassL;
    OnePoleFilter lowPassR;
    
    // Filter cutoff frequencies (vintage 80s rack delay character)
    static constexpr double HIGH_PASS_FREQ = 80.0;    // 80 Hz - removes low rumble
    static constexpr double LOW_PASS_FREQ = 9000.0;   // 9 kHz - warm high-end rolloff
    static constexpr double ANTI_ALIAS_FREQ = 10000.0; // Anti-aliasing before downsample
    
    // Channel crosstalk (authentic 80s analog bleed between L/R)
    // -40 dB = 0.01 (1% bleed) - typical for quality rack units
    static constexpr float CROSSTALK_AMOUNT = 0.01f;
    
    // Convert milliseconds to samples at internal rate
    int msToSamples(int ms) const;
    
    // Process single sample through delay line (at internal rate)
    void processInternalSample(float inputL, float inputR,
                               float& outputL, float& outputR,
                               int delaySamples);
};

//------------------------------------------------------------------------
} // namespace Yonie