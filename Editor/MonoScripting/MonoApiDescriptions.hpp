#pragma once

#include <string>
#include <mono/metadata/appdomain.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/debug-helpers.h>
#include <Core/VarTypes/Base.hpp>
#include <MonoScripting/MonoClassDesc.hpp>

namespace VSGEditor {
	class MonoApiDescriptions {
	private:
		MonoAssembly* _assembly;
		MonoImage* _image;
		std::vector<VSGE::MonoClassDesc*> _api_class_descs;
	public:

		MonoApiDescriptions();

		~MonoApiDescriptions();

		void Load();

		const std::vector<VSGE::MonoClassDesc*>& GetClassesDescs() const;
	};
}