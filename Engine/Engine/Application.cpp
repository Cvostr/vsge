#include "Application.hpp"
#include <algorithm>
#include "Graphics/Shader.hpp"
#include "Graphics/Fonts.hpp"
#include <Core/Time.hpp>

using namespace VSGE;

Application* Application::_this = nullptr;

extern Application* VSGEMain();

void Application::OnUpdate() {
	TimePerf::Get()->Tick();
	for (auto Layer : mLayers) {
		Layer->OnUpdate();
	}
}

void Application::OnSDL2Event(void* event) {
    for (auto layer : mLayers) {
        layer->OnSDL2Event(event);
    }
}
void Application::OnEvent(IWindowEvent& event) {
    for (auto layer : mLayers) {
        layer->OnWindowEvent(event);
    }
}

void Application::Run() {
	mRunning = true;

	while (mRunning) {
		this->mWindow->PollEvents();
		OnUpdate();
	}
}

void Application::Stop() { 
	mRunning = false; 
	RemoveAllLayers();
}

void Application::AddLayer(IApplicationLayer* layer) {
	//Check, if this layer already registered
	for (auto _layer : mLayers) {
		if (_layer == layer)
			return;
	}

	mLayers.push_back(layer);
	layer->OnAttach();
}

void Application::RemoveLayer(IApplicationLayer* layer) {
	layer->OnDetach();
	tApplicationLayerList::iterator it = std::remove(mLayers.begin(), mLayers.end(), layer);
	mLayers.pop_back();
	delete layer;
}

void Application::RemoveAllLayers() {
	while (mLayers.size() > 0)
		RemoveLayer(mLayers[0]);
}

int main(int argc, char* argv[]) {
	//Create singleton
	ShaderStorage storage;
	GlyphManager glyphs;
	TimePerf time;

	Application* app = VSGEMain();

	app->Run();

	return 0;
}
