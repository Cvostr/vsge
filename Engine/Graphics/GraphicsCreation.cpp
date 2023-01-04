#include <Engine/Application.hpp>

#include "Texture.hpp"
#include "Mesh.hpp"

#include "Vulkan/VulkanTexture.hpp"

#include "Vulkan/VulkanMesh.hpp"

#include "Vulkan/VulkanBuffer.hpp"

using namespace VSGE;

Texture* VSGE::CreateTexture() {
	GraphicsApi api = Application::Get()->GetGraphicsApi();

	if (api == GRAPHICS_API_VULKAN)
		return new VulkanTexture;

	return nullptr;
}

Mesh* VSGE::CreateMesh() {
	GraphicsApi api = Application::Get()->GetGraphicsApi();

	if (api == GRAPHICS_API_VULKAN)
		return new VulkanMesh;
	return nullptr;
}

GpuBuffer* VSGE::CreateGpuBuffer(GpuBufferType type) {
	GraphicsApi api = Application::Get()->GetGraphicsApi();

	if (api == GRAPHICS_API_VULKAN)
		return new VulkanBuffer(type);
	return nullptr;
}