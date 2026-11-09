#include "ApiBindings.hpp"
#include <Core/Time.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static float GetDeltaTime() {
	return TimePerf::Get()->GetDeltaTime();
}

void VSGE::BindTimeApi() {
	MonoScriptingLayer::AddInternalCall("Time::GetDeltaTime()", GetDeltaTime);
}