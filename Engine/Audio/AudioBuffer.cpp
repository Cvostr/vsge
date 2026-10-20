#include "AudioBuffer.hpp"
#include <AL/al.h>
#include <Core/Logger.hpp>

#define uint8 stb_internal_uint8
#define int8  stb_internal_int8
#define uint16 stb_internal_uint16
#define int16 stb_internal_int16
#define uint32 stb_internal_uint32
#define int32 stb_internal_int32

#define STB_VORBIS_IMPLEMENTATION
#include "stb/stb_vorbis.h"

#undef uint8
#undef int8
#undef uint16
#undef int16
#undef uint32
#undef int32

using namespace VSGE;

AudioBuffer::AudioBuffer() :
	_created(false),
	_audio_buffer(0),
    _channels(1),
    _bitsPerSample(16),
    _frequency(0),
    _duration(0),
    _bitRate(0)
{

}

AudioBuffer::~AudioBuffer() {
    Destroy();
}

void AudioBuffer::Create() {
    if (!_created) {
        alGenBuffers(1, &_audio_buffer);
        _created = true;
    }
}

void AudioBuffer::Destroy() {
	if (_created) {
		alDeleteBuffers(1, &_audio_buffer);
        _audio_buffer = 0;
		_created = false;
	}
}

uint32 AudioBuffer::GetBuffer() {
    return _audio_buffer;
}

uint32 AudioBuffer::GetFrequency() {
    return _frequency;
}

int AudioBuffer::GetBitsPerSample() {
    return _bitsPerSample;
}

uint32 AudioBuffer::GetChannels() {
    return _channels;
}

uint32 AudioBuffer::GetDuration() {
    return _duration;
}

uint32 AudioBuffer::GetBitRate() {
    return _bitRate;
}

bool AudioBuffer::loadBuffer(byte* buffer, size_t size)
{
    if (memcmp(buffer, "OggS", 4) == 0) 
    {
        return loadBufferOgg(buffer, size);
    }
    
    return loadBufferWAV(buffer);
}

bool AudioBuffer::loadBufferOgg(byte* buffer, size_t size)
{
    int channels;
    int sampleRate;
    short *decoded;
    int samples = stb_vorbis_decode_memory(buffer, size, &channels, &sampleRate, &decoded);

    if (samples == -1) {
        return false;
    }

    ALenum alFormat;
    ALsizei bufferSize;

    // stb_vorbis output is always 16bit
    if (channels == 1)
        alFormat = AL_FORMAT_MONO16;
    else if (channels == 2)
        alFormat = AL_FORMAT_STEREO16;
    else {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Channels count " << channels << " unsupported!\n";
        return false;
    }

    // total size of decoded buffer
    bufferSize = samples * channels * sizeof(short);

    // Fill internal variables
    _frequency = sampleRate;
    _channels = channels;
    _bitsPerSample = 16;
    _bitRate = _bitsPerSample * _channels * _frequency / 1000;
    _duration = bufferSize / _bitRate * 8;

    // Create OpenAL internal buffer
    Create();

    alBufferData(_audio_buffer, alFormat, decoded, bufferSize, sampleRate);

    free(decoded);

    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error loading " << err << "\n";
        return false;
    }

    return true;
}

bool AudioBuffer::loadBufferWAV(byte* buffer) 
{
	Create();

    int format = AL_FORMAT_MONO16;

    if (buffer[12] != 'f' || buffer[13] != 'm' || buffer[14] != 't' || buffer[15] != ' ')
    {
        return false;
    }
    if (buffer[21] != 0 || buffer[20] != 1)
    {
        return false;
    }

    _channels = static_cast<uint32>(buffer[23] << 8);
    _channels |= buffer[22];

    _frequency = static_cast<uint32>(buffer[27] << 24);
    _frequency |= static_cast<uint32>(buffer[26] << 16);
    _frequency |= static_cast<uint32>(buffer[25] << 8);
    _frequency |= static_cast<uint32>(buffer[24]);

    _bitsPerSample = buffer[35] << 8;
    _bitsPerSample |= buffer[34];

    if (_bitsPerSample == 8)
    {
        if (_channels == 1)
            format = AL_FORMAT_MONO8;
        else if (_channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if (_bitsPerSample == 16)
    {
        if (_channels == 1)
            format = AL_FORMAT_MONO16;
        else if (_channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    if (!format)
    {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Incompatible format (" << _channels << ", " << _bitsPerSample << ")\n";
        return false;
    }

    int start_offset = 34;

    //read 4 bytes, until "data" header found
    while (buffer[start_offset] != 'd' || buffer[start_offset + 1] != 'a' || buffer[start_offset + 2] != 't' || buffer[start_offset + 3] != 'a')
        start_offset += 1;
    //Read size
    start_offset += 4;
    //Calculate size
    uint32 _size = buffer[start_offset + 3] << 24; //Getting size, 32 bit value
    _size |= buffer[start_offset + 2] << 16;
    _size |= buffer[start_offset + 1] << 8;
    _size |= buffer[start_offset];

    start_offset += 4;

    _bitRate = _bitsPerSample * _channels * _frequency / 1000;
    _duration = _size / _bitRate * 8;

    //Send data to OpenAL
    alBufferData(_audio_buffer, format, static_cast<void*>(buffer + start_offset), _size, static_cast<int>(_frequency));
    int err = alGetError();
    if (err != AL_NO_ERROR)
    {
        Logger::Log(LogType::LOG_TYPE_ERROR) << "Error loading " << err << "\n";
        return false;
    }

    return true;
}