#pragma once

#include "../Base/ImVulkanTexture.hpp"

namespace VSGEditor {
	class EditorIcons {
	private:
		ImguiVulkanTexture _backBtnIcon;
		ImguiVulkanTexture _dirIcon;
		ImguiVulkanTexture _unknownFile;
		ImguiVulkanTexture _3DModelIcon;
		ImguiVulkanTexture _sceneIcon;
		ImguiVulkanTexture _scriptIcon;
		ImguiVulkanTexture _audioIcon;

		VSGE::VulkanSampler _sampler;

		static EditorIcons* _this;
	public:

		EditorIcons();
		~EditorIcons();

		static EditorIcons* Get() {
			return _this;
		}
		void LoadIcons();
		void FreeIcons();

		void* GetUnknownFileIcon() {
			return _unknownFile.imtexture;
		}

		void* GetDirIcon() {
			return _dirIcon.imtexture;
		}

		void* GetBackBtnIcon() {
			return _backBtnIcon.imtexture;
		}

		void* GetSceneIcon() {
			return _sceneIcon.imtexture;
		}

		void* Get3DModelIcon() {
			return _3DModelIcon.imtexture;
		}

		void* GetScriptIcon() {
			return _scriptIcon.imtexture;
		}

		void* GetAudioIcon() {
			return _audioIcon.imtexture;
		}
	};
}