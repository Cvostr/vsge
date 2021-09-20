#pragma once

#include <string>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/debug-helpers.h>
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

		void Release();

		void LoadFromFile(const std::string& path);
		MonoClass* GetClassDescription(const std::string& class_name, const std::string& namespace_name = "");
		MonoMethod* GetMethodByDescription(MonoMethodDesc* method_desc);
	};
}