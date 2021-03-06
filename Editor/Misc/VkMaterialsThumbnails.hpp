#pragma once

#include <Graphics/Vulkan/Rendering/VulkanGBufferRenderer.hpp>
#include <Graphics/Vulkan/Rendering/VulkanDeferredLight.hpp>
#include <Graphics/Vulkan/Rendering/Postprocess/VulkanGammaCorrection.hpp>
#include <Graphics/LightsBuffer.hpp>
#include <Graphics/Vulkan/VulkanCommandBuffer.hpp>
#include <Graphics/Vulkan/VulkanSynchronization.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Entity.hpp>
#include "../Base/ImVulkanTexture.hpp"

#define THUMBNAIL_TEXTURE_SIZE 512

class VkMaterialThumbnail {
public:
	std::string material_name;
	VSGE::VulkanTexture* texture;
	ImTextureID imtexture;
	bool deleted;

	VkMaterialThumbnail(VSGE::VulkanSampler* sampler);

	~VkMaterialThumbnail();
};

class VkMaterialsThumbnails {
private:
	static VkMaterialsThumbnails* _this;

	VSGE::VulkanGBufferRenderer* _gbuffer;
	VSGE::VulkanDeferredLight* _light;

	VSGE::Camera* _camera;
	VSGE::Scene* _thumb_scene;
	VSGE::Entity* _thumb_entity;
	VSGE::tEntityList _entities_to_render;
	VSGE::tEntityList _particles_to_render;
	VSGE::VulkanBuffer* _transform_buffer;
	VSGE::VulkanBuffer* _storage_buffer;
	VSGE::LightsBuffer* _lights_buffer;
	VSGE::VulkanSampler* _sampler;

	VSGE::VulkanGammaCorrection* _gamma_correction;

	std::vector<VkMaterialThumbnail*> _thumbnails;
	std::vector<std::string> _queued;


	VkMaterialThumbnail* GetPlace(const std::string& name);
public:

	VkMaterialsThumbnails();
	~VkMaterialsThumbnails();

	static VkMaterialsThumbnails* Get() {
		return _this;
	}

	void Create();
	void Destroy();

	void RecreateAll();
	void CreateThumbnail(const std::string& material_name);
	VkMaterialThumbnail* GetMaterialThumbnail(const std::string& material_name);
	ImTextureID GetMaterialThumbnailTexture(const std::string& material_name);

	void RecordCmdBuffer(VSGE::VulkanCommandBuffer* cmdbuf);
};