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
	_resolutionScale = 1.f;

	_root = new View();
	_root->SetAnchors(Vec2(0, 0), Vec2(1, 1));

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
		// Real rendering size
		Vec2i sizes = renderer->GetOutputSizes();
		// Calculate UI height scale
		_resolutionScale = static_cast<float>(sizes.y) / _referenceResolution.y;
		_render_list->SetScale(_resolutionScale);

		Rect screenRect(Vec2(0, 0), Vec2(_referenceResolution.x, _referenceResolution.y));
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
	float dScale = 1.f / _resolutionScale;
	return Vec2(dScale * ires.x, dScale * ires.y);
}