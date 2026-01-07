//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include <vector>
#include <cstring>

namespace Yonie {

//------------------------------------------------------------------------
// DelayBuffer - Stereo circular delay buffer
//------------------------------------------------------------------------
class DelayBuffer
{
public:
    DelayBuffer();
    ~DelayBuffer();
    
    // Prepare buffer for given sample rate and max delay
    void prepare(double sampleRate, int maxDelayMs);
    
    // Process a block of stereo samples with given delay time
    void processStereo(float* leftIn, float* leftOut, 
                      float* rightIn, float* rightOut, 
                      int numSamples, int delayMs);
    
    // Clear the buffer
    void reset();
    
private:
    std::vector<float> bufferL;
    std::vector<float> bufferR;
    int writePos;
    int maxSamples;
    double currentSampleRate;
    
    // Convert milliseconds to samples
    int msToSamples(int ms) const;
};

//------------------------------------------------------------------------
} // namespace Yonie