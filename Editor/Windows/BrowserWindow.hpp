#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vector>
#include <string>
#include "../Base/ImVulkanTexture.hpp"

namespace VSGEditor {

	struct FileEntry {
		bool isDir;
		std::string abs_path;
		std::string rel_path;
		std::string name;
		std::string ext;
		bool is3dModel() {
			return !ext.compare(".rp3m");
		}
		bool isTexture() {
			return !ext.compare(".DDS") || !ext.compare(".dds");
		}
		bool is3dWorld() {
			return !ext.compare(".scn");
		}
		bool isMaterial() {
			return !ext.compare(".rpmat");
		}
	};

	typedef std::vector<FileEntry> tFileEntryList;

	class FileBrowserWindow : public EditorWindow {
	private:
		tFileEntryList mFiles;
		std::string mCurrentDir;
		std::string mRootDir;

		FileEntry* entryToDelete;

		VSGE::VulkanSampler sampler;
		struct {
		
			ImguiVulkanTexture mBackBtnIcon;
			ImguiVulkanTexture mDirIcon;
			ImguiVulkanTexture mUnknownFile;
			ImguiVulkanTexture m3DModelIcon;
			ImguiVulkanTexture mSceneIcon;

			
		}FileIcons;

	public:

		FileBrowserWindow(std::string RootDir);

		void OnDrawWindow();

		void Regroup(uint32 width, uint32 height);

		void cd_up();
		void cd(std::string DirName);
		void SetDirectory(std::string Dir);
		void UpdateDirectoryContent();
		void OpenFile(const FileEntry& Entry);
		void DeleteFile(FileEntry* Entry);
		const std::string& GetCurrentDir() {
			return mCurrentDir;
		}
	};
}