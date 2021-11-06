#include "ApiBindings.hpp"
#include <Graphics/Material.hpp>
#include <MonoScripting/MonoScriptingLayer.hpp>

using namespace VSGE;

static void SetIntValue(Material* mat, MonoString* key, int value) {
	std::string skey = std::string(mono_string_to_utf8(key));
	mat->SetParameter(skey, value);
}

static void SetFloatValue(Material* mat, MonoString* key, float value) {
	std::string skey = std::string(mono_string_to_utf8(key));
	mat->SetParameter(skey, value);
}

static void SetColorValue(Material* mat, MonoString* key, Color value) {
	std::string skey = std::string(mono_string_to_utf8(key));
	mat->SetParameter(skey, value);
}

void VSGE::BindMaterialObjectApi() {
	mono_add_internal_call("Material::i_SetIntValue(ulong,string,single)", SetIntValue);
	mono_add_internal_call("Material::i_SetFloatValue(ulong,string,single)", SetFloatValue);
	mono_add_internal_call("Material::i_SetColorValue(ulong,string,Color)", SetColorValue);
}