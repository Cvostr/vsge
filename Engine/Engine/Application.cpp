#include "Application.hpp"
#include <algorithm>
#include <Core/Time.hpp>

using namespace VSGE;

Application* Application::_this = nullptr;

extern Application* VSGEMain();

Application::Application(ApplicationCreateInfo descr) :
	_running(false),
	_window(new Window()),
	_description(descr),
	_queuedEventsMutex(new Mutex())
{
	_this = this;
}

Application::~Application() {
	SAFE_RELEASE(_window)
	SAFE_RELEASE(_queuedEventsMutex)
}

void Application::OnUpdate() {
	TimePerf::Get()->Tick();
	_queuedEventsMutex->Lock();
	for (uint32 event_i = 0; event_i < _queuedEvents.size(); event_i ++) {
		IEvent* event_ptr = _queuedEvents[0];
		OnEvent(*(event_ptr));
		std::remove(_queuedEvents.begin(), _queuedEvents.end(), event_ptr);
		_queuedEvents.pop_back();
		delete event_ptr;
	}
	_queuedEventsMutex->Release();

	for (auto Layer : _layers) {
		Layer->OnUpdate();
	}
}

void Application::OnSDL2Event(void* event) {
    for (auto layer : _layers) {
        layer->OnSDL2Event(event);
    }
}
void Application::OnEvent(const IEvent& event) {
	for (auto layer : _layers) {
		if (_running) {
			layer->OnEvent(event);
		}
	}
}

void Application::QueueEvent(const IEvent* event) {
	_queuedEventsMutex->Lock();
	_queuedEvents.push_back((IEvent*)event);
	_queuedEventsMutex->Release();
}

void Application::Run() {
	_running = true;

	while (_running) {
		this->_window->PollEvents();
		OnUpdate();
	}
}

void Application::Stop() { 
	_running = false;
	_window->DestroyWindow();
	RemoveAllLayers();
}

void Application::AddLayer(IApplicationLayer* layer) {
	//Check, if this layer already registered
	for (auto _layer : _layers) {
		if (_layer == layer)
			return;
	}

	_layers.push_back(layer);
	layer->OnAttach();
}

void Application::RemoveLayer(IApplicationLayer* layer) {
	layer->OnDetach();
	tApplicationLayerList::iterator it = std::remove(_layers.begin(), _layers.end(), layer);
	_layers.pop_back();
	delete layer;
}

void Application::RemoveAllLayers() {
	while (_layers.size() > 0)
		RemoveLayer(_layers[0]);
}

int main(int argc, char* argv[]) {
	Application* app = VSGEMain();

	app->Run();

	return 0;
}
