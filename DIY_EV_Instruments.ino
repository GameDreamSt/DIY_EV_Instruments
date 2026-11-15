
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
bool ledsActive;

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

    InitializeGauges();

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
    //TickLEDs();
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
