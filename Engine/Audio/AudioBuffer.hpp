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

		void Create();

		void Destroy();

		bool loadBufferWAV(byte* buffer);

		uint32 GetBuffer() {
			return _audio_buffer;
		}
		/// <summary>
		/// Get sample rate of clip
		/// </summary>
		/// <returns></returns>
		uint32 GetFrequency() {
			return _frequency;
		}
		/// <summary>
		/// Get pits per sample value of clip
		/// </summary>
		/// <returns></returns>
		int GetBitsPerSample() {
			return _bitsPerSample;
		}
		/// <summary>
		/// Get channel count of clip
		/// </summary>
		/// <returns></returns>
		uint32 GetChannels() {
			return _channels;
		}
		/// <summary>
		/// Get duration of clip in milliseconds
		/// </summary>
		/// <returns></returns>
		uint32 GetDuration() {
			return _duration;
		}
	};
}