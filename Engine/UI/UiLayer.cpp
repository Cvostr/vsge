#include "UiLayer.hpp"

#include <Engine/Application.hpp>
#include <Graphics/Renderer.hpp>
//TEMPORARY
#include "Widgets/ImageView.hpp"
#include "Widgets/TextView.hpp"

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
	img->SetBounds(Rect(Vec2(100, 100), Vec2(100, 50)));
	img->SetRotation(45);
	_root->AddChild(img);

	TextView* tv = new TextView;
	tv->SetBounds(Rect(Vec2(300, 100), Vec2(100, 50)));
	tv->SetText(L"русский english,.&*#@!()=-_<;:^%$№");
	tv->SetTextColor(Color(0.3f, 0.4f, 0.6f, 1.f));
	_root->AddChild(tv);

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