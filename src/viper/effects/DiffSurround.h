#pragma once

#include <cstdint>
#include "../utils/WaveBuffer.h"

class DiffSurround {
public:
    DiffSurround();
    ~DiffSurround();

    void Process(float *samples, uint32_t size);
    void Reset();
    void SetDelayTime(float delayTime);
    void SetEnable(bool enable);
    void SetSamplingRate(uint32_t samplingRate);

    uint32_t samplingRate;
    bool enable;
    float delayTime;
    WaveBuffer *buffers[2];
};


