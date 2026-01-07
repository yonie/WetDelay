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
, currentSampleRate(44100.0)
{
}

//------------------------------------------------------------------------
DelayBuffer::~DelayBuffer()
{
}

//------------------------------------------------------------------------
void DelayBuffer::prepare(double sampleRate, int maxDelayMs)
{
    currentSampleRate = sampleRate;
    maxSamples = msToSamples(maxDelayMs);
    
    // Allocate buffers
    bufferL.resize(maxSamples, 0.0f);
    bufferR.resize(maxSamples, 0.0f);
    
    writePos = 0;
}

//------------------------------------------------------------------------
void DelayBuffer::processStereo(float* leftIn, float* leftOut,
                                float* rightIn, float* rightOut,
                                int numSamples, int delayMs)
{
    if (bufferL.empty() || bufferR.empty())
        return;
        
    int delaySamples = msToSamples(delayMs);
    
    // Clamp delay to valid range
    delaySamples = std::max(1, std::min(delaySamples, maxSamples - 1));
    
    for (int i = 0; i < numSamples; ++i)
    {
        // Calculate read position (write position minus delay)
        int readPos = writePos - delaySamples;
        if (readPos < 0)
            readPos += maxSamples;
            
        // Write input to buffer
        bufferL[writePos] = leftIn[i];
        bufferR[writePos] = rightIn[i];
        
        // Read delayed output (100% wet)
        leftOut[i] = bufferL[readPos];
        rightOut[i] = bufferR[readPos];
        
        // Advance write position
        writePos++;
        if (writePos >= maxSamples)
            writePos = 0;
    }
}

//------------------------------------------------------------------------
void DelayBuffer::reset()
{
    std::fill(bufferL.begin(), bufferL.end(), 0.0f);
    std::fill(bufferR.begin(), bufferR.end(), 0.0f);
    writePos = 0;
}

//------------------------------------------------------------------------
int DelayBuffer::msToSamples(int ms) const
{
    return static_cast<int>(ms * currentSampleRate / 1000.0);
}

//------------------------------------------------------------------------
} // namespace Yonie