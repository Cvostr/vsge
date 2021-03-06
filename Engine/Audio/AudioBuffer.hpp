#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	class AudioBuffer {
	private:
		bool _created;
		uint32 _audio_buffer;

		uint32 _frequency;
		int _bitsPerSample;
		uint32 _channels;
		uint32 _bitRate;
		uint32 _duration;
	public:
		AudioBuffer();
		~AudioBuffer();

		void Create();

		void Destroy();

		bool loadBufferWAV(byte* buffer);

		uint32 GetBuffer();
		/// <summary>
		/// Get sample rate of clip
		/// </summary>
		/// <returns></returns>
		uint32 GetFrequency();
		/// <summary>
		/// Get pits per sample value of clip
		/// </summary>
		/// <returns></returns>
		int GetBitsPerSample();
		/// <summary>
		/// Get channel count of clip
		/// </summary>
		/// <returns></returns>
		uint32 GetChannels();
		/// <summary>
		/// Get duration of clip in milliseconds
		/// </summary>
		/// <returns></returns>
		uint32 GetDuration();
		/// <summary>
		/// Get bitrate of clip
		/// </summary>
		/// <returns></returns>
		uint32 GetBitRate();
	};
}