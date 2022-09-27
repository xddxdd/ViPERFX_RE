#pragma once

#include "utils/WaveBuffer.h"
#include "effects/SpectrumExtend.h"
#include "effects/Reverberation.h"
#include "effects/DynamicSystem.h"
#include "effects/ViPERClarity.h"
#include "effects/SpeakerCorrection.h"
#include "effects/AnalogX.h"
#include "effects/TubeSimulator.h"
#include "effects/Cure.h"
#include "effects/DiffSurround.h"
#include "effects/VHE.h"
#include "utils/AdaptiveBuffer.h"
#include "effects/Convolver.h"
#include "effects/ViPERDDC.h"
#include "effects/IIRFilter.h"
#include "effects/ColorfulMusic.h"
#include "effects/FETCompressor.h"
#include "effects/ViPERBass.h"
#include "effects/SoftwareLimiter.h"
#include "effects/PlaybackGain.h"
#include "../ViPER4Android.h"

class ViPER {
public:
    ViPER();
    ~ViPER();

    void processBuffer(float *buffer, uint32_t size);
    // TODO: Parameter types/names
    void DispatchCommand(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6, int param_7);
    void ResetAllEffects();

    bool update_status;
    uint64_t process_time_ms;
    bool init_ok;
    bool enabled;
    bool force_enabled;
    uint32_t samplingRate;

    // Effects
    AdaptiveBuffer *adaptiveBuffer;
    WaveBuffer *waveBuffer;
    bool fetcomp_enabled;
    Convolver *convolver;
    VHE *vhe;
    ViPERDDC *viperDdc;
    SpectrumExtend *spectrumExtend;
    IIRFilter *iirFilter;
    ColorfulMusic *colorfulMusic;
    Reverberation *reverberation;
    PlaybackGain *playbackGain;
    FETCompressor *fetCompressor;
    DynamicSystem *dynamicSystem;
    ViPERBass *viperBass;
    ViPERClarity *viperClarity;
    DiffSurround *diffSurround;
    Cure *cure;
    TubeSimulator *tubeSimulator;
    AnalogX *analogX;
    SpeakerCorrection *speakerCorrection;
    SoftwareLimiter *softwareLimiters[2];

    float frame_scale;
    float left_pan;
    float right_pan;
};
