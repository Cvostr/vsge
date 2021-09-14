#include <Engine/Application.hpp>

#include "Texture.hpp"
#include "Mesh.hpp"

#include "Vulkan/VulkanTexture.hpp"
#include "D3D12/D3D12Texture.hpp"

#include "Vulkan/VulkanMesh.hpp"

#include "Vulkan/VulkanBuffer.hpp"

using namespace VSGE;

Texture* VSGE::CreateTexture() {
	GraphicsApi api = Application::Get()->GetGraphicsApi();

	if (api == GRAPHICS_API_VULKAN)
		return new VulkanTexture;
	else if (api == GRAPHICS_API_D3D12)
		return new D3D12Texture;

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