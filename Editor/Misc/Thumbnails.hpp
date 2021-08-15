#pragma once

#include <vector>
#include <string>
#include "../Base/ImVulkanTexture.hpp"

typedef std::pair<std::string, ImguiVulkanTexture*> STRIMGVKT;

class TextureThumbnails {
private:
	static TextureThumbnails* _this;

	VSGE::VulkanSampler* sampler;
	std::vector<STRIMGVKT*> _textureResources;
	ImguiVulkanTexture* _emptyTextureIcon;
public:

	TextureThumbnails() {
		_this = this;
		sampler = nullptr;
		_emptyTextureIcon = nullptr;
	}

	static TextureThumbnails* Get() {
		return _this;
	}

	ImguiVulkanTexture* GetTextureResource(const std::string& fname);

	ImguiVulkanTexture* GetCheckerboardTexture() { return _emptyTextureIcon; }
};