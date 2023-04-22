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
		MonoAssembly* m_assembly;
		MonoImage* m_image;
		std::vector<MonoClassDesc*> m_class_descs;
		void ClearDescs();
	public:

		MonoScriptBlob();
		~MonoScriptBlob();

		MonoAssembly* GetAssembly();
		MonoImage* GetImage();
		MonoClassDesc* GetMonoClassDesc(const std::string& class_name, const std::string& namespace_name = "");
		const std::vector<MonoClassDesc*>& GetClassesDescs() const;

		void Release();

		//0 - success
		//1 - assembly load failed
		//2 - image load failed
		int LoadFromFile(const std::string& path);
		MonoMethod* GetMethodByDescription(MonoMethodDesc* method_desc);
		void BuildMonoClassDescsList();
	};
}