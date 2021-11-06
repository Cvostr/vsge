#pragma once

#include <string>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/debug-helpers.h>
#include <Core/VarTypes/Base.hpp>
#include "MonoClassDesc.hpp"

namespace VSGE {

	class MonoScriptBlob {
	private:
		MonoAssembly* _assembly;
		MonoImage* _image;
		std::vector<MonoClassDesc> _class_descs;
	public:

		MonoScriptBlob();
		~MonoScriptBlob();

		MonoAssembly* GetAssembly();
		MonoImage* GetImage();
		MonoClassDesc* GetMonoClassDesc(const std::string& class_name);

		void Release();

		void LoadFromFile(const std::string& path);
		MonoClass* GetClassDescription(const std::string& class_name, const std::string& namespace_name = "");
		MonoMethod* GetMethodByDescription(MonoMethodDesc* method_desc);
		void BuildMonoClassDescsList();
	};
}