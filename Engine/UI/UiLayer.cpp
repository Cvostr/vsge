#include "UiLayer.hpp"

#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>
//TEMPORARY
#include "Widgets/ImageView.hpp"

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
	SpriteView* img = new SpriteView;
	img->SetSprite("test_bc");
	img->SetAnchorMin(Vec2(0.25f));
	img->SetAnchorMax(Vec2(0.75f));
	_root->AddChild(img);

}
void UiLayer::OnUpdate() {
	IRenderer* renderer = Application::Get()->GetRenderer();
	if (renderer) {
		Vec2i sizes = renderer->GetOutputSizes();
		_root->SetBounds(Rect(Vec2(0, 0), Vec2(sizes.x, sizes.y)));
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

View* UiLayer::GetRootView() {
	return _root;
}