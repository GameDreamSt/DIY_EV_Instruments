
#include "LEDPanel.h"

#include "CANCommunication.h"
#include "Pinout.h"

#include "EVLib/EVTime.h"
#include "EVLib/Timer.h"

#include <Arduino.h>

EVData ledStats;
Timer ledTimer = Timer(1);
bool ledActive = false;

float LEDTestTime;

void InitializeLEDs()
{
    pinMode(PIN_COOLANT_LED, OUTPUT);
    pinMode(PIN_BATTERY_LED, OUTPUT);

    LEDTestTime = 1;
}

void TickLEDs()
{
    if (LEDTestTime > 0)
    {
        LEDTestTime -= deltaTime;
        digitalWrite(PIN_COOLANT_LED, true);
        digitalWrite(PIN_BATTERY_LED, true);
        return;
    }

    ledStats = GetEVData();
    if (ledTimer.HasTriggered())
        ledActive = !ledActive;

    if (ledStats.GetTemperatureStatus01() > 0.7f)
        digitalWrite(PIN_COOLANT_LED, ledActive);
    else
        digitalWrite(PIN_COOLANT_LED, false);

    if (ledStats.DCDC_Failed())
        digitalWrite(PIN_BATTERY_LED, ledActive);
    else
        digitalWrite(PIN_BATTERY_LED, false);
}