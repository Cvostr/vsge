#include "UiLayer.hpp"

using namespace VSGE;

UiLayer* UiLayer::_this = nullptr;

UiLayer::UiLayer() {
	_this = this;

	_root = new View();
}
UiLayer::~UiLayer() {
	SAFE_RELEASE(_root)
}

void UiLayer::OnAttach() {

}
void UiLayer::OnUpdate() {

}
void UiLayer::OnDetach() {

}

View* UiLayer::GetRootView() {
	return _root;
}