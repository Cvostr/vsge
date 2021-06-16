#pragma once

#include "../Engine/ApplicationLayer.hpp"
#include <AL/alc.h>

namespace VSGE {
	class AudioLayer : public IApplicationLayer {
	private:
		ALCdevice* _openal_device;
		ALCcontext* _openal_context;
	public:
		AudioLayer();

		void OnAttach();
		void OnUpdate();
		void OnDetach();
	};
}