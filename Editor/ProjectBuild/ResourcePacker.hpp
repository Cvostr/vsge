#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <mpi/Core/Guid.hpp>
#include <Core/Threading/Thread.hpp>
#include "TextureCompressor.hpp"

namespace VSGEditor {

	struct MapEntry {
		std::string resource_name;
		Guid resourceId;
		int type;
		uint32 offset;
		uint32 size;
		uint32 bundle_index;
	};

	class ResourcePacker : public VSGE::Thread {
	private:
		// State
		std::string _output_log;
		std::size_t _total;
		std::size_t _completed;

		// Settings
		std::string _output_dir;
		std::string _bundle_name_prefix;
		uint32 _bundle_file_split_size;
		TargetTextureFormat _textureCompressFormat;

		std::vector<MapEntry> _map_entries;
		uint32 _current_bundle;
		uint32 _written_bytes;
		bool _finished;

		std::ofstream _bundle_stream;

		void WriteFileToBundle(byte* data, uint32 size);
		void CheckForBundleOverflow();

		void ResetStageProgress(std::size_t total);

		bool CompileAndWriteScripts();

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
		size_t GetTotal();
		size_t GetCompleted();

		uint32 GetBundleFileSplitSizeMegs();
		void SetBundleFileSplitSizeMegs(uint32 size);

		TargetTextureFormat GetTextureCompressFormat();
		void SetTextureCompressFormat(TargetTextureFormat format);
	};
}