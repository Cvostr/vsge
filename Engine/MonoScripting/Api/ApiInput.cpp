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
static Vec2i GetMouseCursorPos() {
	return Input::Get()->GetMouseCursorPos();
}
static Vec2i GetMouseCursorDelta() {
	return Input::Get()->GetMouseCursorDelta();
}

void VSGE::BindInputApi() {
	mono_add_internal_call("Input::IsMouseButtonPressed(MouseButton)", IsMouseButtonPressed);
	mono_add_internal_call("Input::IsMouseButtonHold(MouseButton)", IsMouseButtonHold);
	mono_add_internal_call("Input::IsKeyPressed(KeyCode)", IsKeyPressed);
	mono_add_internal_call("Input::IsKeyHold(KeyCode)", IsKeyHold);
	mono_add_internal_call("Input::GetMouseCursorPos()", GetMouseCursorPos);
	mono_add_internal_call("Input::GetMouseCursorDelta()", GetMouseCursorDelta);
}