//
// Created by mart on 7/25/21.
//

#include <ctime>
#include "ViPER.h"
#include "Effect.h"
#include "constants.h"

ViPER::ViPER() {
    VIPER_LOGI("Welcome to ViPER FX");
    VIPER_LOGI("Current version is %s %s", VERSION_STRING, VERSION_CODENAME);

    this->adaptiveBuffer = new AdaptiveBuffer_F32(2, 4096);
    this->waveBuffer = new WaveBuffer_I32(2, 4096);

    this->convolver = new Convolver();
//    this->convolver->SetEnable(false);
//    this->convolver->SetSamplingRate(this->sampleRate);
//    this->convolver->Reset();

    this->vhe = new VHE();
    this->vhe->SetEnable(false);
    this->vhe->SetSamplingRate(this->sampleRate);
    this->vhe->Reset();

    this->viperDdc = new ViPERDDC();
//    this->viperDdc->SetEnable(false);
//    this->viperDdc->SetSamplingRate(this->sampleRate);
//    this->viperDdc->Reset();

    this->spectrumExtend = new SpectrumExtend();
    this->spectrumExtend->SetEnable(false);
    this->spectrumExtend->SetSamplingRate(this->sampleRate);
    this->spectrumExtend->SetReferenceFrequency(7600);
    this->spectrumExtend->SetExciter(0);
    this->spectrumExtend->Reset();

    this->iirFilter = new IIRFilter();
//    this->iirFilter->SetEnable(false);
//    this->iirFilter->SetSamplingRate(this->sampleRate);
//    this->iirFilter->Reset();

    this->colorfulMusic = new ColorfulMusic();
//    this->colorfulMusic->SetEnable(false);
//    this->colorfulMusic->SetSamplingRate(this->sampleRate);
//    this->colorfulMusic->Reset();

    this->reverberation = new Reverberation();
    this->reverberation->SetEnable(false);
    this->reverberation->SetSamplingRate(this->sampleRate);
    this->reverberation->Reset();

    this->playbackGain = new PlaybackGain();
//    this->playbackGain->SetEnable(false);
//    this->playbackGain->SetSamplingRate(this->sampleRate);
//    this->playbackGain->Reset();

    this->fetCompressor = new FETCompressor();
//    this->fetCompressor->SetEnable(false);
//    this->fetCompressor->SetSamplingRate(this->sampleRate);
//    this->fetCompressor->Reset();

    this->dynamicSystem = new DynamicSystem();
    this->dynamicSystem->SetEnable(false);
    this->dynamicSystem->SetSamplingRate(this->sampleRate);
    this->dynamicSystem->Reset();

    this->viperBass = new ViPERBass();
//    this->viperBass->SetEnable(false);
//    this->viperBass->SetSamplingRate(this->sampleRate);
//    this->viperBass->Reset();

    this->viperClarity = new ViPERClarity();
    this->viperClarity->SetEnable(false);
    this->viperClarity->SetSamplingRate(this->sampleRate);
    this->viperClarity->Reset();

    this->diffSurround = new DiffSurround();
    this->diffSurround->SetEnable(false);
    this->diffSurround->SetSamplingRate(this->sampleRate);
    this->diffSurround->Reset();

    this->cure = new Cure();
    this->cure->SetEnable(false);
    this->cure->SetSamplingRate(this->sampleRate);
    this->cure->Reset();

    this->tubeSimulator = new TubeSimulator();
//    this->tubeSimulator->SetEnable(false);
//    this->tubeSimulator->SetSamplingRate(this->sampleRate);
    this->tubeSimulator->Reset();

    this->analogX = new AnalogX();
//    this->analogX->SetEnable(false);
    this->analogX->SetSamplingRate(this->sampleRate);
    this->analogX->SetProcessingModel(0);
    this->analogX->Reset();

    this->speakerCorrection = new SpeakerCorrection();
    this->speakerCorrection->SetEnable(false);
    this->speakerCorrection->SetSamplingRate(this->sampleRate);
    this->speakerCorrection->Reset();

    for (auto &softwareLimiter: this->softwareLimiters) {
        softwareLimiter = new SoftwareLimiter();
//        softwareLimiter->ResetLimiter();
    }

    this->init_ok = true;

    this->enabled = false;
    this->force_enabled = false;
    this->mode = ViPER_FX_TYPE_NONE; // 0
}

ViPER::~ViPER() {
    delete this->adaptiveBuffer;
    this->adaptiveBuffer = nullptr;

    delete this->waveBuffer;
    this->waveBuffer = nullptr;

    delete this->convolver;
    this->convolver = nullptr;

    delete this->vhe;
    this->vhe = nullptr;

    delete this->viperDdc;
    this->viperDdc = nullptr;

    delete this->spectrumExtend;
    this->spectrumExtend = nullptr;

    delete this->iirFilter;
    this->iirFilter = nullptr;

    delete this->colorfulMusic;
    this->colorfulMusic = nullptr;

    delete this->reverberation;
    this->reverberation = nullptr;

    delete this->playbackGain;
    this->playbackGain = nullptr;

    delete this->fetCompressor;
    this->fetCompressor = nullptr;

    delete this->dynamicSystem;
    this->dynamicSystem = nullptr;

    delete this->viperBass;
    this->viperBass = nullptr;

    delete this->viperClarity;
    this->viperClarity = nullptr;

    delete this->diffSurround;
    this->diffSurround = nullptr;

    delete this->cure;
    this->cure = nullptr;

    delete this->tubeSimulator;
    this->tubeSimulator = nullptr;

    delete this->analogX;
    this->analogX = nullptr;

    delete this->speakerCorrection;
    this->speakerCorrection = nullptr;

    for (auto &softwareLimiter: softwareLimiters) {
        delete softwareLimiter;
        softwareLimiter = nullptr;
    }
}

int32_t
ViPER::command(uint32_t cmdCode, uint32_t cmdSize, void *pCmdData, uint32_t *replySize, void *pReplyData) {
    switch (cmdCode) {
        case EFFECT_CMD_ENABLE: {
            if (!this->enabled) {
                ResetAllEffects();
                break;
            }
            return 0;
        }
        case EFFECT_CMD_RESET: {
            ResetAllEffects();
            break;
        }
        case EFFECT_CMD_SET_CONFIG: {
            if (cmdSize != 64 || *replySize != 4) {
                return -EINVAL;
            }

            auto currentSampleRate = this->sampleRate;

            *(int32_t *) pReplyData = this->configure((effect_config_t *) pCmdData);
            if (*(int32_t *) pReplyData == 0) {
                if (currentSampleRate != this->sampleRate) {
                    ResetAllEffects();
                }
            }

            return 0;
        }
        case EFFECT_CMD_SET_PARAM: {
            auto pCmdParam = (effect_param_t *) pCmdData;

            if (pCmdParam->psize != sizeof(int32_t)) {
                *(int32_t *) pReplyData = -EINVAL;
                return 0;
            }

            // TODO: implement
        }
        case EFFECT_CMD_GET_PARAM: {
            auto *pCmdParam = (effect_param_t *) pCmdData;
            auto *pReplyParam = (effect_param_t *) pReplyData;

            if (pCmdParam->psize != sizeof(int32_t)) break;

            switch (*(int *) pCmdParam->data) {
                case PARAM_GET_DRIVER_VERSION: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = 0x2050004; // As original, change as needed
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_NEONENABLED: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = 1; // TODO: check if neon is enabled
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_ENABLED: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = this->enabled;
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_CONFIGURE: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = this->configureOk;
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_DRVCANWORK: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = this->init_ok;
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_STREAMING: {
                    struct timeval time{};
                    gettimeofday(&time, nullptr);

                    // TODO: Do some calculations

                    return 0;
                }
                case PARAM_GET_EFFECT_TYPE: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = this->mode;
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_SAMPLINGRATE: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(uint32_t *) pReplyParam->data = this->sampleRate;
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_CONVUSABLE: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
                    *(int32_t *) pReplyParam->data = 1; // TODO: Figure out what is this
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
                case PARAM_GET_CONVKNLID: {
                    pReplyParam->status = 0;
                    //pReplyParam->psize = sizeof(int32_t); // TODO
                    pReplyParam->vsize = sizeof(int32_t);
//                *(int32_t *) pReplyParam->data = this->convolver->GetKernelID(); // TODO: Uncomment when function is implemented
                    *replySize = 0x14; // As original, TODO: calculate correctly
                    return 0;
                }
            }
        }
    }

    return this->Effect::command(cmdCode, cmdSize, pCmdData, replySize, pReplyData);
}

void ViPER::processBuffer(float *buffer, int frameSize) {
    // TODO
}

void ViPER::DispatchCommand(int param_1, int param_2, int param_3, int param_4, int param_5, int param_6,
                            int param_7) {
    // TODO
}

void ViPER::ResetAllEffects() {
    if (this->adaptiveBuffer != nullptr) {
//        this->adaptiveBuffer->FlushBuffer();
    }
    if (this->waveBuffer != nullptr) {
        this->waveBuffer->Reset();
    }
    if (this->convolver != nullptr) {
//        this->convolver->SetSamplingRate(this->sampleRate);
//        this->convolver->Reset();
    }
    if (this->vhe != nullptr) {
        this->vhe->SetSamplingRate(this->sampleRate);
        this->vhe->Reset();
    }
    if (this->viperDdc != nullptr) {
//        this->viperDdc->SetSamplingRate(this->sampleRate);
//        this->viperDdc->Reset();
    }
    if (this->spectrumExtend != nullptr) {
        this->spectrumExtend->SetSamplingRate(this->sampleRate);
        this->spectrumExtend->Reset();
    }
    if (this->iirFilter != nullptr) {
//        this->iirFilter->SetSamplingRate(this->sampleRate);
//        this->iirFilter->Reset();
    }
    if (this->colorfulMusic != nullptr) {
//        this->colorfulMusic->SetSamplingRate(this->sampleRate);
//        this->colorfulMusic->Reset();
    }
    if (this->reverberation != nullptr) {
        this->reverberation->SetSamplingRate(this->sampleRate);
        this->reverberation->Reset();
    }
    if (this->playbackGain != nullptr) {
//        this->playbackGain->SetSamplingRate(this->sampleRate);
//        this->playbackGain->Reset();
    }
    if (this->fetCompressor != nullptr) {
//        this->fetCompressor->SetSamplingRate(this->sampleRate);
//        this->fetCompressor->Reset();
    }
    if (this->dynamicSystem != nullptr) {
        this->dynamicSystem->SetSamplingRate(this->sampleRate);
        this->dynamicSystem->Reset();
    }
    if (this->viperBass != nullptr) {
//        this->viperBass->SetSamplingRate(this->sampleRate);
//        this->viperBass->Reset();
    }
    if (this->viperClarity != nullptr) {
        this->viperClarity->SetSamplingRate(this->sampleRate);
        this->viperClarity->Reset();
    }
    if (this->diffSurround != nullptr) {
        this->diffSurround->SetSamplingRate(this->sampleRate);
        this->diffSurround->Reset();
    }
    if (this->cure != nullptr) {
        this->cure->SetSamplingRate(this->sampleRate);
        this->cure->Reset();
    }
    if (this->tubeSimulator != nullptr) {
//        this->tubeSimulator->Reset();
    }
    if (this->analogX != nullptr) {
        this->analogX->SetSamplingRate(this->sampleRate);
        this->analogX->Reset();
    }
    if (this->speakerCorrection != nullptr) {
        this->speakerCorrection->SetSamplingRate(this->sampleRate);
        this->speakerCorrection->Reset();
    }
    for (auto &softwareLimiter: softwareLimiters) {
        if (softwareLimiter != nullptr) {
//            softwareLimiter->Reset();
        }
    }
}
