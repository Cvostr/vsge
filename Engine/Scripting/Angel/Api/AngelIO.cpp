#include "AngelApi.hpp"
#include <Scripting/Angel/AngelScriptLayer.hpp>
#include <Core/Logger.hpp>

using namespace VSGE;

void printToConsole(asIScriptGeneric* gen)
{
	unsigned int args = gen->GetArgCount();
	std::string out = "";

	for (unsigned int arg_i = 0; arg_i < args; arg_i++) {
		void* ref = gen->GetArgAddress(arg_i);
		int typeId = gen->GetArgTypeId(arg_i);

		switch (typeId) {
		case asTYPEID_INT32: {
			int* v = static_cast<int*>(ref);
			out += std::to_string(*v);
			break;
		}
		case asTYPEID_FLOAT: {
			float* f = static_cast<float*>(ref);
			out += std::to_string(*f);
			break;
		}
		case asTYPEID_BOOL: {
			bool* f = static_cast<bool*>(ref);
			out += (*f == true) ? "true" : "false";
			break;
		}
		case STRING_TYPEID: {
			std::string* str = static_cast<std::string*>(ref);
			out += *str;
			break;
		}
		case VEC3_TYPEID: {
			Vec3* vec = static_cast<Vec3*>(ref);
			out += "(" + std::to_string(vec->x) + ", " + std::to_string(vec->y) + ", " + std::to_string(vec->z) + ")";
			break;
		}
		}
		//Add space
		out += " ";
	}

	Logger::Log(LogType::LOG_TYPE_INFO) << out;
}

void VSGE::BindIO() {
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	std::string args = "?&in";
	for (unsigned int i = 0; i < 9; i++) {
		std::string func = "void print(" + args + ")";
		layer->RegisterGlobalFunction(func, asFUNCTION(printToConsole), asCALL_GENERIC);
		args += ", ?&in";
	}
}