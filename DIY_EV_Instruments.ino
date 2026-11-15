
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
#include "src/LEDPanel.h"

#include "FS.h"
#include <LittleFS.h>
#include "src/FileUtility.h"

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

    InitializeLEDs();
    InitializeGauges();

    if (!LittleFS.begin(false))
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
