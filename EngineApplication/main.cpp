#include <Engine/Application.hpp>
#include "api_examples/vulkan_rect_example.hpp"
#include "api_examples/d3d12_rect_example.hpp"

using namespace VSGE;

Application* VSGEMain() {
	ApplicationCreateInfo descr = { "Development testing", 1 };
	descr.graphicsApi = GRAPHICS_API_VULKAN;

	auto app = new Application(descr);

	app->GetWindow().CreateWindow(1280, 720, "Test");

	//app->AddLayer(new VSGETest::VulkanRectTestLayer);
	app->AddLayer(new VSGETest::VulkanRectTestLayer);

	return app;
}