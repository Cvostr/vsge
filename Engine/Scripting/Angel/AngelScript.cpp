#include "AngelScript.hpp"

using namespace VSGE;

asITypeInfo* AngelScript::GetMainClassTypeInfo() {
	return _main_class;
}

asIScriptFunction* AngelScript::getFuncOnMainClass(const char* decl) {
	return _main_class->GetMethodByDecl(decl);
}