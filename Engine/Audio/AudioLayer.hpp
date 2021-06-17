#pragma once

#include "../Engine/ApplicationLayer.hpp"
#include <AL/alc.h>
#include <Math/Vec3.hpp>

namespace VSGE {
	class AudioLayer : public IApplicationLayer {
	private:
		ALCdevice* _openal_device;
		ALCcontext* _openal_context;
		bool _created;

		static AudioLayer* _this;
	public:
		AudioLayer();

		static AudioLayer* Get() {
			return _this;
		}

		void OnAttach();
		void OnUpdate();
		void OnDetach();

		void SetListenerPos(const Vec3& pos);
		void SetListenerOrientation(const Vec3& front, const Vec3& up);
		void SetListenerVolume(float volume);
	};
}