#include "UiLayer.hpp"
#include <Input/Input.hpp>

#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>

using namespace VSGE;

UiLayer* UiLayer::_this = nullptr;

UiLayer::UiLayer() {
	_this = this;

	// Full HD by default
	_referenceResolution = Vec2i(1920, 1080);

	_root = new View();
	_root->SetAnchors(Vec2(0,0), Vec2(1,1));

	_render_list = new UiRenderList;
}
UiLayer::~UiLayer() {
	SAFE_RELEASE(_root);
	SAFE_RELEASE_ARR(_render_list)
}

void UiLayer::OnAttach() {
}
void UiLayer::OnUpdate() {
	IRenderer* renderer = Application::Get()->GetRenderer();
	if (renderer) {
		Vec2i sizes = renderer->GetOutputSizes();
		Rect screenRect(Vec2(0, 0), Vec2(sizes.x, sizes.y));
		_root->UpdateGlobalRect(screenRect);
		DrawAll();
	}
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

void UiLayer::SetReferenceResolution(const Vec2i& resolution) {
	_referenceResolution = resolution;
}

View* UiLayer::GetRootView() {
	return _root;
}

Vec2 UiLayer::GetMouseCursorPos() 
{
	Vec2i ires = Input::Get()->GetMouseCursorPos();
	// TODO: Add virtual resolution support
	return Vec2(ires.x, ires.y);
}