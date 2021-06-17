#include "AudioBuffer.hpp"
#include <AL/al.h>
#include <Core/Logger.hpp>

using namespace VSGE;

AudioBuffer::AudioBuffer() :
	_created(false),
	_audio_buffer(0),
    _channels(1),
    _bitsPerSample(16),
    _frequency(0)
{

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
		_created = false;
	}
}

bool AudioBuffer::loadBufferWAV(byte* buffer) {
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
    int _size = buffer[start_offset + 3] << 24; //Getting size, 32 bit value
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