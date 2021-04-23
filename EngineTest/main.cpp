#include <Engine/Application.hpp>
#include "api_examples/vulkan_rect_example.hpp"


using namespace VSGE;

Application* VSGEMain() {
	ApplicationDescription descr = { "Development testing", 1 };

	auto app = new Application(descr);

	app->GetWindow().CreateWindow(1280, 720, "Test", SDL_WINDOW_VULKAN);

	app->AddLayer(new VSGETest::VulkanRectTestLayer);

	return app;
}
