#pragma once

#include <string>
#include <mono/metadata/appdomain.h>
#include <Core/VarTypes/Base.hpp>

namespace VSGE {
	class MonoScriptBlob {
	private:
		MonoAssembly* _assembly;
		MonoImage* _image;
	public:

		MonoScriptBlob();
		~MonoScriptBlob();

		MonoAssembly* GetAssembly();
		MonoImage* GetImage();

		void LoadFromFile(const std::string& path);
		MonoClass* GetClass(const std::string& class_name);
	};
}