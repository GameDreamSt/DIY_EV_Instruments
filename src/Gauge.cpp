
#include "CANCommunication.h"
#include "Gauge.h"
#include "Pinout.h"
#include "Commands.h"

#include "EVLib/EVTime.h"
#include "EVLib/MathUtils.h"
#include "EVLib/SerialPrint.h"

#include <Arduino.h>

#define MAX_RPM 7000

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
    if (channel < 0)
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

// ***********************************

EVData stats;
Gauge tachGauge, coolantGauge, fuelGauge;

enum TackSweepMode
{
    Done,
    GoingUp,
    GoingDown,
};
TackSweepMode tackSweepMode;
float tachValue;
const float tackSweepTime = 2;

void InitializeGauges()
{
    tachValue = 0;
    tackSweepMode = TackSweepMode::GoingUp;

    tachGauge = Gauge(PIN_TACH, CHANNEL_TACH);
    tachGauge.SetCalibration(CHANNEL_MAX_TACH);

    coolantGauge = Gauge(PIN_COOLANT_GAUGE, CHANNEL_COOL);
    coolantGauge.SetCalibration(CHANNEL_MAX_COOL);

    fuelGauge = Gauge(PIN_FUEL_GAUGE, CHANNEL_FUEL);
    fuelGauge.SetCalibration(CHANNEL_MAX_FUEL);
}

void TickTach()
{
    float calibration = 0;
    if (commands::TryGetGaugeCalibration(CHANNEL_TACH, calibration))
        tachGauge.SetCalibration(calibration);

    switch (tackSweepMode)
    {
    case TackSweepMode::Done:
        tachValue = stats.RPM / (float)MAX_RPM;
        break;

    case TackSweepMode::GoingUp:
        tachValue += deltaTime / tackSweepTime;
        if (tachValue >= 1)
        {
            tackSweepMode = TackSweepMode::GoingDown;
            tachValue = 1;
        }
        break;

    case TackSweepMode::GoingDown:
        tachValue -= deltaTime / tackSweepTime;
        if (tachValue < 0)
        {
            tackSweepMode = TackSweepMode::Done;
            tachValue = 0;
        }
        break;
    }

    if (commands::TryGetGaugeValueRatio(CHANNEL_TACH, tachValue))
        tachGauge.SetValueRatio(tachValue);
    else
        tachGauge.SetValueRatio(tachValue);
}

void TickCoolantGauge()
{
    float calibration = 0;
    if (commands::TryGetGaugeCalibration(CHANNEL_COOL, calibration))
        coolantGauge.SetCalibration(calibration);

    float value = 0;
    if (commands::TryGetGaugeValueRatio(CHANNEL_COOL, value))
        coolantGauge.SetValueRatio(value);
    else
        coolantGauge.SetValueRatio(stats.GetTemperatureStatus01());
}

void TickFuelGauge()
{
    float calibration = 0;
    if (commands::TryGetGaugeCalibration(CHANNEL_FUEL, calibration))
        fuelGauge.SetCalibration(calibration);

    float value = 0;
    if (commands::TryGetGaugeValueRatio(CHANNEL_FUEL, value))
        fuelGauge.SetValueRatio(value);
    else
        fuelGauge.SetValueRatio(stats.GetSOC01());
}

void TickGauges()
{
    stats = GetEVData();
    TickTach();
    TickCoolantGauge();
    TickFuelGauge();
}