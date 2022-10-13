#include "MultiBiquad.h"
#include <cmath>

MultiBiquad::MultiBiquad() {
    this->a1 = 0.0;
    this->a2 = 0.0;
    this->b0 = 0.0;
    this->b1 = 0.0;
    this->b2 = 0.0;
    this->x1 = 0.0;
    this->x2 = 0.0;
    this->y1 = 0.0;
    this->y2 = 0.0;
}

double MultiBiquad::ProcessSample(double sample) {
    double out =
            sample * this->b0 +
            this->x1 * this->b1 +
            this->x2 * this->b2 +
            this->y1 * this->a1 +
            this->y2 * this->a2;

    this->x2 = this->x1;
    this->x1 = sample;
    this->y2 = this->y1;
    this->y1 = out;

    return out;
}

void
MultiBiquad::RefreshFilter(FilterType type, float gainAmp, float frequency, uint32_t samplingRate, float qFactor, bool param_7) {
    float gain;

    if (type - 5 < 3) { // type - 5 < 3 is always true... right?
        gain = pow(10.0f, gainAmp / 40.0f);
    } else {
        gain = pow(10.0f, gainAmp / 20.0f);
    }

    double omega = (2.0 * M_PI * (double) frequency) / (double) samplingRate;
    double sinOmega = sin(omega);
    double cosOmega = cos(omega);

    double y;
    double z;

    if (type - 6 < 2) {
        y = sinOmega / 2.0 * sqrt((1.0 / ((double) gain * 2.0)) * (1.0 / (double) qFactor - 1.0) + 2.0);
        z = sqrt((double) gain) * y;
    } else if (!param_7) {
        y = sinOmega / ((double) qFactor / 2.0);
        z = -1.0;
    } else {
        y = sinh(((double) qFactor * (log(2.0) / 2.0) * omega) / sinOmega);
        z = -1.0;
    }

    double a0;
    double a1;
    double a2;
    double b0;
    double b1;
    double b2;

    switch (type) {
        case LOWPASS: {
            a0 = 1.0 + y;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y;
            b0 = (1.0 - cosOmega) / 2.0;
            b1 = 1.0 - cosOmega;
            b2 = (1.0 - cosOmega) / 2.0;
            break;
        }
        case HIGHPASS: {
            a0 = 1.0 + y;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y;
            b0 = (1.0 + cosOmega) / 2.0;
            b1 = -1.0 - cosOmega;
            b2 = (1.0 + cosOmega) / 2.0;
            break;
        }
        case BANDPASS: {
            a0 = 1.0 + y;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y;
            b0 = y;
            b1 = 0.0;
            b2 = -y;
            break;
        }
        case BANDSTOP: {
            a0 = 1.0 + y;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y;
            b0 = 1.0;
            b1 = -2.0 * cosOmega;
            b2 = 1.0;
            break;
        }
        case ALLPASS: {
            a0 = 1.0 + y;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y;
            b0 = 1.0 - y;
            b1 = -2.0 * cosOmega;
            b2 = 1.0 + y;
            break;
        }
        case PEAK: {
            a0 = 1.0 + y / (double) gain;
            a1 = -2.0 * cosOmega;
            a2 = 1.0 - y / (double) gain;
            b0 = 1.0 + y * (double) gain;
            b1 = -2.0 * cosOmega;
            b2 = 1.0 - y * (double) gain;
            break;
        }
        case LOWSHELF: {
            double tmp1 = (gain + 1.0) - (gain - 1.0) * cosOmega;
            double tmp2 = (gain + 1.0) + (gain - 1.0) * cosOmega;
            a1 = ((gain - 1.0) + (gain + 1.0) * cosOmega) * -2.0;
            a2 = tmp2 - z;
            b1 = (gain * 2.0) * ((gain - 1.0) - (gain + 1.0) * cosOmega);
            a0 = tmp2 + z;
            b0 = (tmp1 + z) * gain;
            b2 = (tmp1 - z) * gain;
            break;
        }
        case HIGHSHELF: {
            double tmp1 = (gain + 1.0) + (gain - 1.0) * cosOmega;
            double tmp2 = (gain + 1.0) - (gain - 1.0) * cosOmega;
            a2 = tmp2 - z;
            a0 = tmp2 + z;
            a1 = ((gain - 1.0) - (gain + 1.0) * cosOmega) * 2.0;
            b1 = gain * -2.0 * ((gain - 1.0) + (gain + 1.0) * cosOmega);
            b0 = (tmp1 + z) * gain;
            b2 = (tmp1 - z) * gain;
            break;
        }
    }

    this->x2 = 0.0;
    this->x1 = 0.0;
    this->y2 = 0.0;
    this->y1 = 0.0;

    this->a1 = -a1 / a0;
    this->a2 = -a2 / a0;
    this->b0 = b0 / a0;
    this->b1 = b1 / a0;
    this->b2 = b2 / a0;
}
