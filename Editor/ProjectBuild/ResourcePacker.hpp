#pragma once

#include <Core/VarTypes/Base.hpp>
#include <string>

namespace VSGEditor {
	class ResourcePacker {
	private:
		std::string _output_file;

		uint32 _written_bytes;
	public:

		ResourcePacker();

		void SetOutputBundleFilePath(const std::string& path);

		void Write();
	};
}