#include <stdint.h>

#include "FS.h"
#include "WAVFileReader.h"

#include <Arduino.h>

WAVFileReader::WAVFileReader(File *fp)
{
    valid = false;

    // read the WAV header
    fp->read((uint8_t*)&m_wav_header, sizeof(wav_header_t));
    // sanity check the bit depth
    if (m_wav_header.bit_depth != 16)
    {
        Serial.printf("ERROR: bit depth %d is not supported\n", m_wav_header.bit_depth);
    }
    if (m_wav_header.num_channels != 1)
    {
        Serial.printf("ERROR: channels %d is not supported\n", m_wav_header.num_channels);
    }
    Serial.printf("fmt_chunk_size=%d, audio_format=%d, num_channels=%d, sample_rate=%d, sample_alignment=%d, bit_depth=%d, data_bytes=%d\n",
             m_wav_header.fmt_chunk_size, m_wav_header.audio_format, m_wav_header.num_channels, m_wav_header.sample_rate, m_wav_header.sample_alignment, m_wav_header.bit_depth, m_wav_header.data_bytes);

    audioDataBytes = m_wav_header.data_bytes;

    if(audioDataBytes <= 0)
    {
        fp->close();
        return;
    }

    audioData = new uint8_t[audioDataBytes];
    
    fp->seek(sizeof(wav_header_t), fs::SeekSet);
    Serial.printf("Available data: %d\n", fp->available());

    fp->read(audioData, audioDataBytes);
    fp->close();

    valid = true;
}
