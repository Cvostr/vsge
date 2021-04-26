#include "d3d12_rect_example.hpp"
#include <Graphics/D3D12/D3D12RAPI.hpp>
#include <Engine/Application.hpp>

using namespace VSGETest;
using namespace VSGE;

void D3D12RectTestLayer::OnAttach() {
	Application* app = Application::Get();

	D3D12RAPI* d3d = new D3D12RAPI;
	d3d->Create(&app->GetWindow());
}
void D3D12RectTestLayer::OnUpdate() {

}
void D3D12RectTestLayer::OnDetach() {

}
void D3D12RectTestLayer::OnWindowEvent(VSGE::IWindowEvent& event) {

}