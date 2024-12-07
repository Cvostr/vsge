#include "ApiBindings.hpp"
#include <Graphics/Material.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>
#include <Core/Logger.hpp>

using namespace VSGE;

static void SetIntValue(Material* mat, MonoString* key, int value) 
{
	char* pKey = mono_string_to_utf8(key);
	std::string sKey = std::string(pKey);
	mono_free(pKey);

	mat->SetParameter(sKey, value);
}

static void SetFloatValue(Material* mat, MonoString* key, float value) 
{
	char* pKey = mono_string_to_utf8(key);
	std::string sKey = std::string(pKey);
	mono_free(pKey);

	mat->SetParameter(sKey, value);
}

static void SetColorValue(Material* mat, MonoString* key, Color value) 
{
	char* pKey = mono_string_to_utf8(key);
	std::string sKey = std::string(pKey);
	mono_free(pKey);

	mat->SetParameter(sKey, value);
}

static void SetMaterialTexture(Material* mat, MonoString* key, Resource* res)
{
	if (res == nullptr) 
	{
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Setting NULL texture to material slot" << "\n";
		return;
	}

	char* pKey = mono_string_to_utf8(key);
	std::string sKey = std::string(pKey);
	mono_free(pKey);

	ResourceReference ref;
	ref.SetResource(res->getId());

	mat->SetTexture(sKey, ref);
}

void VSGE::BindMaterialObjectApi() {
	MonoScriptingLayer::AddInternalCall("Material::i_SetIntValue(ulong,string,single)", SetIntValue);
	MonoScriptingLayer::AddInternalCall("Material::i_SetFloatValue(ulong,string,single)", SetFloatValue);
	MonoScriptingLayer::AddInternalCall("Material::i_SetColorValue(ulong,string,Color)", SetColorValue);
	MonoScriptingLayer::AddInternalCall("Material::i_SetMaterialTexture(ulong,string,ulong)", SetMaterialTexture);
}