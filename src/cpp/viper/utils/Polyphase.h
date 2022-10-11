#pragma once

#include <cstdint>
#include "FIR.h"
#include "WaveBuffer.h"

class Polyphase {
public:
    Polyphase(int param_1);
    ~Polyphase();

    uint32_t GetLatency();
    uint32_t Process(float *samples, uint32_t size);
    void Reset();
    void SetSamplingRate(uint32_t samplingRate);

private:
    FIR fir1;
    FIR fir2;
    WaveBuffer *waveBuffer1;
    WaveBuffer *waveBuffer2;
    float *buffer;
    uint32_t samplingRate;
};


