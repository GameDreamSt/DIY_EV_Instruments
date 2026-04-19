
#include "src/EVLib/EVTime.h"
#include "src/EVLib/SerialPrint.h"
#include "src/EVLib/SerialReader.h"
#include "src/EVLib/Timer.h"

#include "src/Commands.h"
#include "src/Display.h"
#include "src/Gauge.h"
#include "src/Pinout.h"
#include "src/CANCommunication.h"

#include "FS.h"
#include <LittleFS.h>
#include "src/FileUtility.h"

#include "src/Audio/PDMOutput.h"
#include "src/WAV/WAVFileReader.h"

#define FORMAT_LITTLEFS_IF_FAILED true

Timer ledTimer = Timer(2);
Timer gaugeTimer = Timer(1);
bool ledsActive;

Gauge tachGauge, coolantGauge, fuelGauge;

AudioOutput *audioOutput = NULL;
WAVFileReader *wavFileReader = NULL;

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

    i2s_pin_config_t i2s_pdm_pins = {.bck_io_num = I2S_PIN_NO_CHANGE,
                                     .ws_io_num = I2S_PIN_NO_CHANGE,
                                     .data_out_num = PIN_AUDIO,
                                     .data_in_num = I2S_PIN_NO_CHANGE};
    audioOutput = new PDMOutput(i2s_pdm_pins);

    if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED))
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    ListDirectory("/");

    File fp = LittleFS.open("/chime.wav", "r");

    if (!fp)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    wavFileReader = new WAVFileReader(&fp);

    if (!wavFileReader->IsValid())
        return;
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

void TickAudio()
{
    if (!commands::TriggerChime())
        return;

    if (wavFileReader == nullptr)
    {
        PrintSerialMessage("WAV reader is null!");
        return;
    }

    uint8_t *sampleData = wavFileReader->GetSamples();
    int sampleBytes = wavFileReader->GetSampleCount();

    if (sampleData == nullptr || sampleBytes <= 0)
    {
        PrintSerialMessage("No samples available to play!");
        return;
    }

    if (audioOutput == nullptr)
    {
        PrintSerialMessage("Audio player is null!");
        return;
    }

    int16_t *samples = (int16_t *)sampleData;
    int sampleCount = sampleBytes / sizeof(int16_t);

    Serial.printf("Playing %d samples\n", sampleCount);
    int timeStart = millis();

    audioOutput->start(wavFileReader->GetSampleRate());
    audioOutput->write(samples, sampleCount);
    audioOutput->stop();

    Serial.printf("Done. Took %d ms", millis() - timeStart);
}
