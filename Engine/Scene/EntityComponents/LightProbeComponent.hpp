#pragma once

#include "../IEntityComponent.hpp"

namespace VSGE {

	class LightProbeComponent : public IEntityComponent {
	private:

	public:
		LightProbeComponent();
		DEFINE_ENTITY_COMPONENT(ENTITY_COMPONENT_LIGHT_PROBE, "Light Probe")

	};
}