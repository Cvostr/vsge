#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>
#include <fstream>

#include <Core/Threading/Thread.hpp>

namespace VSGEditor {

	struct MapEntry {
		std::string resource_name;
		int type;
		uint32 offset;
		uint32 size;
		uint32 bundle_index;
	};

	class ResourcePacker : public VSGE::Thread {
	private:
		std::string _output_log;
		std::ofstream _bundle_stream;

		std::string _output_dir;

		std::string _bundle_name_prefix;
		uint32 _bundle_file_split_size;

		std::vector<MapEntry> _map_entries;
		uint32 _current_bundle;
		uint32 _written_bytes;
		bool _finished;

		void WriteFileToBundle(byte* data, uint32 size);
		void CheckForBundleOverflow();

		template<typename T>
		ResourcePacker& operator<<(T value) {
			_output_log += value;
			return *this;
		}

	public:

		ResourcePacker();

		void SetOutputBundleDirectoryPath(const std::string& path);

		void Write();

		void THRFunc();

		bool IsFinished();

		const std::string& GetOutput();
	};
}