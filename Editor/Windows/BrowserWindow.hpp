#pragma once

#include "../Base/ImGuiWindow.hpp"
#include <vector>
#include <string>
#include <Resources/Resource.hpp>
#include <Core/ByteSerialize.hpp>

namespace VSGEditor {

	struct FileEntry {
		bool isDir;
		std::string abs_path; //full path on disk
		std::string rel_path;
		std::string name; //name of file with ext
		std::string ext; //file extension
		std::string directory; //directory path of file
		bool is3dModel() {
			return !ext.compare(".vs3m");
		}
		bool isTexture() {
			return !ext.compare(".DDS") ||
				!ext.compare(".dds") || 
				!ext.compare(".PNG") ||
				!ext.compare(".png") || 
				!ext.compare(".JPG") || 
				!ext.compare(".jpg") ||
				!ext.compare(".vstx");
		}
		bool is3dWorld() {
			return !ext.compare(".scn");
		}
		bool isMaterial() {
			return !ext.compare(".vsmt");
		}
		bool isPrefab() {
			return !ext.compare(".prefab");
		}
		bool isScript() {
			return !ext.compare(".cs");
		}
		bool isAudio() {
			return !ext.compare(".wav");
		}
	};

	typedef std::vector<FileEntry> tFileEntryList;

	class FileBrowserWindow : public EditorWindow {
	private:
		tFileEntryList _files;
		std::string _currentDir;
		std::string _rootDir;

		int _itemsSize;

		FileEntry* renaming_entry;
		std::string rename_file_name;
		bool openRenameDialog;

		VSGE::ResourceType new_resource_type;

		void RemoveFile(const std::string& path);
		void RenameFileDialog(FileEntry* Entry);
		void CreateResourceDialog();
		VSGE::ByteSerialize* GetBaseContentToWrite(VSGE::ResourceType res_type, const std::string& res_name);
	public:

		FileBrowserWindow(std::string RootDir);

		void OnDrawWindow();

		void cd_up();
		void cd(std::string DirName);
		void SetDirectory(std::string Dir);
		void UpdateDirectoryContent();
		void OpenFile(const FileEntry& Entry);

		const std::string& GetCurrentDir() {
			return _currentDir;
		}
	};
}