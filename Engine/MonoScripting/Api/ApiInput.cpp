#include "ApiBindings.hpp"
#include <Input/Input.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static bool IsMouseButtonPressed(MouseButton button) {
	return Input::Get()->IsMouseButtonPressed(button);
}
static bool IsMouseButtonHold(MouseButton button) {
	return Input::Get()->IsMouseButtonHold(button);
}
static bool IsKeyPressed(KeyCode key) {
	return Input::Get()->IsKeyPressed(key);
}
static bool IsKeyHold(KeyCode key) {
	return Input::Get()->IsKeyHold(key);
}
static void GetMouseCursorPos(Vec2i* out) {
	*out = Input::Get()->GetMouseCursorPos();
}
static void GetMouseCursorDelta(Vec2i* out) {
	*out = Input::Get()->GetMouseCursorDelta();
}
static void GetMouseScroll(Vec2* out) {
	*out = Input::Get()->GetMouseScroll();
}

void VSGE::BindInputApi() {
	MonoScriptingLayer::AddInternalCall("Input::IsMouseButtonPressed(MouseButton)", IsMouseButtonPressed);
	MonoScriptingLayer::AddInternalCall("Input::IsMouseButtonHold(MouseButton)", IsMouseButtonHold);
	MonoScriptingLayer::AddInternalCall("Input::IsKeyPressed(KeyCode)", IsKeyPressed);
	MonoScriptingLayer::AddInternalCall("Input::IsKeyHold(KeyCode)", IsKeyHold);
	MonoScriptingLayer::AddInternalCall("Input::i_GetMouseScroll(Vec2&)", GetMouseScroll);
	MonoScriptingLayer::AddInternalCall("Input::i_GetMouseCursorPos(Vec2i&)", GetMouseCursorPos);
	MonoScriptingLayer::AddInternalCall("Input::i_GetMouseCursorDelta(Vec2i&)", GetMouseCursorDelta);
}