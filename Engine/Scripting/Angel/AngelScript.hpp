#pragma once

#include <Core/VarTypes/Base.hpp>
#include <angelscript/angelscript.h>

namespace VSGE {
	class AngelScript {
	private:
		asITypeInfo* _main_class;
	public:
		asITypeInfo* GetMainClassTypeInfo();

		asIScriptFunction* getFuncOnMainClass(const char* decl);
	};
}