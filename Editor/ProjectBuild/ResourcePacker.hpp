#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>

namespace VSGEditor {

	struct MapEntry {
		std::string resource_name;
		uint32 offset;
		uint32 size;
		uint32 bundle_index;
	};

	class ResourcePacker {
	private:
		std::string _output_dir;

		std::string _bundle_name_prefix;
		uint32 _bundle_file_split_size;

		std::vector<MapEntry> _map_entries;
		uint32 _current_bundle;
		uint32 _written_bytes;

		void WriteFileToBundle(byte* data, uint32 size);

	public:

		ResourcePacker();

		void SetOutputBundleDirectoryPath(const std::string& path);

		void Write();
	};
}