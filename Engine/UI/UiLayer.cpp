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
	DrawAll();
}
void UiLayer::OnDetach() {

}

void UiLayer::DrawAll() {
	_render_list->ClearTasks();
	DrawView(_root);
}

void UiLayer::DrawView(View* view) {
	view->Draw();
	for (auto& child : view->GetChildren()) {
		DrawView(child);
	}
}

UiRenderList* UiLayer::GetRenderList() {
	return _render_list;
}

View* UiLayer::GetRootView() {
	return _root;
}