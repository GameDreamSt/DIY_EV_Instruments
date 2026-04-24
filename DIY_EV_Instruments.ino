
#include "src/EVLib/EVTime.h"
#include "src/EVLib/SerialPrint.h"
#include "src/EVLib/SerialReader.h"
#include "src/EVLib/Timer.h"

#include "src/Commands.h"
#include "src/Display.h"
#include "src/Gauge.h"
#include "src/Pinout.h"
#include "src/CANCommunication.h"
#include "src/Audio.h"

#include "FS.h"
#include <LittleFS.h>
#include "src/FileUtility.h"

#define FORMAT_LITTLEFS_IF_FAILED true

Timer ledTimer = Timer(2);
Timer gaugeTimer = Timer(1);
bool ledsActive;

Gauge tachGauge, coolantGauge, fuelGauge;

void setup()
{
    Serial.begin(115200);

    InitializeSerialReader();
    AddCommand(CommandPointer("setgaugecalibration", commands::CommandGaugeCalibration));
    AddCommand(CommandPointer("setgaugevalue", commands::CommandGaugeValueRatio));
    AddCommand(CommandPointer("playchime", commands::CommandTriggerChime));
    AddCommand(CommandPointer("reset", commands::CommandReset));
    AddCommand(CommandPointer("canerrors", commands::CommandTogglePrintCANErrors));
    AddCommand(CommandPointer("canlogs", commands::CommandTogglePrintCANLogs));

    pinMode(PIN_COOLANT_LED, OUTPUT);
    pinMode(PIN_BATTERY_LED, OUTPUT);

    tachGauge = Gauge(PIN_TACH, CHANNEL_TACH);
    tachGauge.SetCalibration(CHANNEL_MAX_TACH);
    
    coolantGauge = Gauge(PIN_COOLANT_GAUGE, CHANNEL_COOL);
    coolantGauge.SetCalibration(CHANNEL_MAX_COOL);

    fuelGauge = Gauge(PIN_FUEL_GAUGE, CHANNEL_FUEL);
    fuelGauge.SetCalibration(CHANNEL_MAX_FUEL);

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    ListDirectory("/");

    InitializeAudio();
}

void loop()
{
    TickCAN();
    TickTime();
    TickSerialReader();
    TickSerialWriter();
    TickDisplay();
    TickLEDs();
    TickGauges();
    TickAudio();
}

void TickLEDs()
{
    if (!ledTimer.HasTriggered())
        return;

    ledsActive = !ledsActive;

    digitalWrite(PIN_COOLANT_LED, ledsActive);
    digitalWrite(PIN_BATTERY_LED, ledsActive);
}

void TickGauges()
{
    if(!gaugeTimer.HasTriggered())
        return;

    float value = 0;
    if (commands::TryGetGaugeCalibration(CHANNEL_TACH, value))
        tachGauge.SetCalibration(value);
    if (commands::TryGetGaugeValueRatio(CHANNEL_TACH, value))
        tachGauge.SetValueRatio(value);

    if (commands::TryGetGaugeCalibration(CHANNEL_COOL, value))
        coolantGauge.SetCalibration(value);
    if (commands::TryGetGaugeValueRatio(CHANNEL_COOL, value))
        coolantGauge.SetValueRatio(value);

    if (commands::TryGetGaugeCalibration(CHANNEL_FUEL, value))
        fuelGauge.SetCalibration(value);
    if (commands::TryGetGaugeValueRatio(CHANNEL_FUEL, value))
        fuelGauge.SetValueRatio(value);
}
