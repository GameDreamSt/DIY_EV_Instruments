
#include "Gauge.h"
#include "Pinout.h"

#include "EVLib/MathUtils.h"
#include "EVLib/SerialPrint.h"

#include <Arduino.h>

Gauge::Gauge()
{
    channel = -1;
    oldValue = 0;
    calibration = 1;
}

Gauge::Gauge(int setPin, int setChannel)
{
    channel = setChannel;
    oldValue = 0;
    calibration = 1;
    ledcAttachChannel(setPin, LEDC_FREQUENCY, LEDC_RESOLUTION, channel);
}

void Gauge::SetValueRatio(float ratio)
{
    if(channel < 0)
        return;
    oldValue = ratio;

    ratio = Saturate(ratio);
    ratio = Remap(ratio, 0, 1, 0, calibration);
    int duty = LEDC_DUTY_MAX * ratio;
    ledcWriteChannel(channel, duty);
}

void Gauge::SetCalibration(float maxRatio)
{
    calibration = Saturate(maxRatio);
    SetValueRatio(oldValue);
}