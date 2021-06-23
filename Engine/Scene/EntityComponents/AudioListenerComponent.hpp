#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {
	class AudioListenerComponent : public IEntityComponent {
	private:
	public:
		AudioListenerComponent() {

		}

		void OnUpdate();
		void OnDestroy();

		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_AUDIO_LISTENER, "Audio Listener")
	};
}