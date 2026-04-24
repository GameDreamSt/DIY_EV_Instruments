
#include "Audio.h"
#include "EVLib/SerialPrint.h"
#include "Commands.h"
#include "Pinout.h"
#include <cstdint>

#include <LittleFS.h>

#include "WAV/WAVFileReader.h"
#include "Audio/PDMOutput.h"

AudioOutput *audioOutput = NULL;
WAVFileReader *wavFileReader = NULL;

void InitializeAudio()
{
    i2s_pin_config_t i2s_pdm_pins = {.bck_io_num = I2S_PIN_NO_CHANGE,
                                     .ws_io_num = I2S_PIN_NO_CHANGE,
                                     .data_out_num = PIN_AUDIO,
                                     .data_in_num = I2S_PIN_NO_CHANGE};

    File fp = LittleFS.open("/chime.wav", "r");

    if (!fp)
    {
        PrintSerialMessage("Failed to open file for reading");
        return;
    }

    wavFileReader = new WAVFileReader(&fp);

    if(!wavFileReader->IsValid())
        return;

    audioOutput = new PDMOutput(i2s_pdm_pins);
}

void TickAudio()
{
    if(wavFileReader == nullptr || !wavFileReader->IsValid())
        return;

    if (!commands::TriggerChime())
        return;

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

    PrintSerialMessage("Playing " + ToString(sampleCount) + " samples\n");
    int timeStart = millis();

    audioOutput->start(wavFileReader->GetSampleRate());
    audioOutput->write(samples, sampleCount);
    audioOutput->stop();

    int timeMS = millis() - timeStart;
    PrintSerialMessage("Done. Took " + ToString(timeMS) + " ms");
}