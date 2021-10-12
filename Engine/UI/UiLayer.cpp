#include "UiLayer.hpp"

using namespace VSGE;

UiLayer* UiLayer::_this = nullptr;

UiLayer::UiLayer() {
	_this = this;

	_root = new View();
	_render_list = new UiRenderList;
}
UiLayer::~UiLayer() {
	SAFE_RELEASE(_root);
	SAFE_RELEASE_ARR(_render_list)
}

void UiLayer::OnAttach() {

}
void UiLayer::OnUpdate() {
	_render_list->ClearTasks();
}
void UiLayer::OnDetach() {

}

UiRenderList* UiLayer::GetRenderList() {
	return _render_list;
}

View* UiLayer::GetRootView() {
	return _root;
}