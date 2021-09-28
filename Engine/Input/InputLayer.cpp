#include "InputLayer.hpp"
#include "Input.hpp"

using namespace VSGE;

InputLayer::InputLayer() {

}
InputLayer::~InputLayer() {

}

void InputLayer::OnAttach() {

}
void InputLayer::OnUpdate() {
	Input::Get()->ClearPressedKeys();
	Input::Get()->UnsetPressedMouseButtons();
}
void InputLayer::OnDetach() {

}
void InputLayer::OnEvent(const VSGE::IEvent& event) {
	DispatchEvent<VSGE::EventMouseMotion>(event, EVENT_FUNC(InputLayer::OnMouseMotion));
	DispatchEvent<VSGE::EventMouseScrolled>(event, EVENT_FUNC(InputLayer::OnMouseScroll));
	DispatchEvent<VSGE::EventMouseButtonDown>(event, EVENT_FUNC(InputLayer::OnMouseButtonDown));
	DispatchEvent<VSGE::EventMouseButtonUp>(event, EVENT_FUNC(InputLayer::OnMouseButtonUp));
	DispatchEvent<VSGE::EventKeyButtonUp>(event, EVENT_FUNC(InputLayer::OnKeyUp));
	DispatchEvent<VSGE::EventKeyButtonDown>(event, EVENT_FUNC(InputLayer::OnKeyDown));
}
void InputLayer::OnMouseMotion(const VSGE::EventMouseMotion& motion) {
	Input::Get()->SetMouseCursorState(
		motion.GetMouseX(), 
		motion.GetMouseY(), 
		motion.GetMouseRelativeX(),
		motion.GetMouseRelativeY());
}
void InputLayer::OnMouseScroll(const VSGE::EventMouseScrolled& scroll) {

}
void InputLayer::OnMouseButtonDown(const VSGE::EventMouseButtonDown& mbd) {
	Input::Get()->SetMouseBtnHold(mbd.GetMouseButton());
}
void InputLayer::OnMouseButtonUp(const VSGE::EventMouseButtonUp& mbu) {
	Input::Get()->UnsetMouseBtnHold(mbu.GetMouseButton());
	Input::Get()->SetMouseBtnPressed(mbu.GetMouseButton());
}
void InputLayer::OnKeyUp(const VSGE::EventKeyButtonUp& kbd) {
	Input::Get()->RemoveHoldKey(kbd.GetKeyCode());
	Input::Get()->AddPressedKey(kbd.GetKeyCode());
}
void InputLayer::OnKeyDown(const VSGE::EventKeyButtonDown& kbd) {
	Input::Get()->AddHoldKey(kbd.GetKeyCode());
}