#include "../ApiBindings.hpp"
#include <Scene/EntityComponents/LightProbeComponent.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void RenderProbe(LightProbeComponent* ptr) {
	ptr->QueueRender();
}

void VSGE::BindEnvProbe()
{
	MonoScriptingLayer::AddInternalCall("EnvironmentProbeComponent::i_Render(ulong)", RenderProbe);
}