#pragma once

#include "WAVFile.h"
#include <stdio.h>

class File;

class WAVFileReader
{
private:
    wav_header_t m_wav_header;
    uint8_t* audioData;
    int audioDataBytes;
    bool valid;

public:
    WAVFileReader(File *fp);
    ~WAVFileReader() { delete audioData; }
    int GetSampleRate() { return m_wav_header.sample_rate; }
    uint8_t* GetSamples() { return audioData; }
    int GetSampleCount() { return audioDataBytes; }
    bool IsValid() { return valid; }
};
