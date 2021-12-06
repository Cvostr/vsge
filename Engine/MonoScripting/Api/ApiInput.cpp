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
	MonoScriptingLayer::AddInternalCall("Input::IsMouseButtonPressed(MouseButton)", IsMouseButtonPressed);
	MonoScriptingLayer::AddInternalCall("Input::IsMouseButtonHold(MouseButton)", IsMouseButtonHold);
	MonoScriptingLayer::AddInternalCall("Input::IsKeyPressed(KeyCode)", IsKeyPressed);
	MonoScriptingLayer::AddInternalCall("Input::IsKeyHold(KeyCode)", IsKeyHold);
	MonoScriptingLayer::AddInternalCall("Input::GetMouseCursorPos()", GetMouseCursorPos);
	MonoScriptingLayer::AddInternalCall("Input::GetMouseCursorDelta()", GetMouseCursorDelta);
}