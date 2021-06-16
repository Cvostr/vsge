#pragma once

#include <Core/VarTypes/Base.hpp>

namespace VSGE {

	class AudioBuffer {
	private:
		bool _created;
		uint32 _audio_buffer;

		uint32 _frequency;
		int _bits;
		uint32 _channels;
	public:
		AudioBuffer();

		void Create();

		void Destroy();

		bool loadBufferWAV(byte* buffer);

		uint32 GetBuffer() {
			return _audio_buffer;
		}

		uint32 GetFrequency() {
			return _frequency;
		}

		int GetBits() {
			return _bits;
		}

		uint32 GetChannels() {
			return _channels;
		}
	};
}