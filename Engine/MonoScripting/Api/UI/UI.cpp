#include "../ApiBindings.hpp"
#include <UI/UiLayer.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetReferenceResolution(int32 width, int32 height) {
	UiLayer::Get()->SetReferenceResolution(Vec2i(width, height));
}

static void GetMouseCursorPos(Vec2 *out) {
	*out = UiLayer::Get()->GetMouseCursorPos();
}

static void GetReferenceResolution(Vec2i* out) {
	*out = UiLayer::Get()->GetReferenceResolution();
}

void VSGE::BindUi()
{
	MonoScriptingLayer::AddInternalCall("UI::i_GetMouseCursorPos(Vec2&)", GetMouseCursorPos);
	MonoScriptingLayer::AddInternalCall("UI::i_SetReferenceResolution(int,int)", SetReferenceResolution);
	MonoScriptingLayer::AddInternalCall("UI::i_GetReferenceResolution(Vec2i&)", GetReferenceResolution);
}