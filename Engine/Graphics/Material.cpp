#include "Material.hpp"
#include "Shader.hpp"
#include <Core/ByteSerialize.hpp>
#include <Core/ByteSolver.hpp>
#include <Core/Logger.hpp>
#include <fstream>
#include "string.h"
#include "MaterialTemplateCache.hpp"

using namespace VSGE;

Material::Material() :
	_template(nullptr),
	_paramsDirty(true),
	_texturesDirty(true),
	_templateChanged(true),
	_descriptors(nullptr)
{}

Material::~Material() {
	DestroyDescriptors();
}

void Material::SetDescriptors(MaterialDescriptor* descriptors) {
	_descriptors = descriptors;
}

MaterialDescriptor* Material::GetDescriptors() {
	return _descriptors;
}


void Material::SetTemplate(MaterialTemplate* mat_template) {
	if (mat_template == nullptr)
		return;

	this->_template = mat_template;
	_templateChanged = true;

	//clear params and textures first
	_materialTextures.clear();
	_materialParams.clear();

	for (auto texture : mat_template->GetTextures()) {
		MaterialTexture* mat_texture = new MaterialTexture;
		mat_texture->_binding = texture->_binding;
		mat_texture->_name = texture->_name;
		this->_materialTextures.push_back(mat_texture);
	}

	for (auto& param : mat_template->GetParams()) {
		this->_materialParams.push_back(param);
	}
}

void Material::SetTemplate(const std::string& template_name) {
	SetTemplate(MaterialTemplateCache::Get()->GetTemplate(template_name));
}

MaterialTexture* Material::GetTextureByName(const std::string& texture_name) {
	for (MaterialTexture* texture : _materialTextures) {
		if (texture->_name == texture_name)
			return texture;
	}

	return nullptr;
}
MaterialParameter* Material::GetParameterByName(const std::string& param_name) {
	for (MaterialParameter& param : _materialParams) {
		if (param.name == param_name)
			return &param;
	}

	return nullptr;
}

void Material::SetTexture(const std::string& texture_name, ResourceReference& texture) {
	MaterialTexture* material_texture = GetTextureByName(texture_name);
	if (!material_texture)
		return;
	material_texture->_resource = texture;
	bool hasTexture = texture.GetResource() != nullptr;
	SetParameter("@has_" + texture_name, hasTexture);
	_texturesDirty = true;
}
void Material::SetParameter(const std::string& parameter_name, Variant value) 
{
	MaterialParameter* matParam = GetParameterByName(parameter_name);
	if (matParam)
	{
		matParam->value = value;
		_paramsDirty = true;
	}
}

uint32 align(uint32 in) {
	return in + (16 - (in % 16));
}

uint32 Material::CopyParamsToBuffer(char** out) {
	*out = new char[1024];
	char* bytes = *out;
	uint32 writtenBytes = 0;

	for (MaterialParameter& param : _materialParams) {
		if (param.value.GetType() == VALUE_TYPE_BOOL) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
			writtenBytes += 4;
		}
		if (param.value.GetType() == VALUE_TYPE_INT32) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
			writtenBytes += 4;
		}
		if (param.value.GetType() == VALUE_TYPE_FLOAT) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
			writtenBytes += 4;
		}
		if (param.value.GetType() == VALUE_TYPE_VEC3F || param.value.GetType() == VALUE_TYPE_VEC3I) {
			writtenBytes = align(writtenBytes);
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 12);
			writtenBytes += 16;
		}
		if (param.value.GetType() == VALUE_TYPE_COLOR) {
			writtenBytes = align(writtenBytes);
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 16);
			writtenBytes += 16;
		}
	}

	return writtenBytes;
}

void Material::Destroy() {
	_materialTextures.clear();
	_materialParams.clear();
	DestroyDescriptors();
}

void Material::DestroyDescriptors() {
	SAFE_RELEASE(_descriptors);
}

void Material::Serialize(const std::string& fpath) {
	ByteSerialize serializer;
	//Записать заголовок
	serializer.WriteBytes("vs_material", 12);

	std::string template_name = "";
	if (GetTemplate()) {
		template_name = GetTemplate()->GetName();
	}

	//Записать название шаблона материала
	serializer.Serialize(template_name);
	//Записать количество текстур и параметров
	serializer.Serialize((uint32)_materialTextures.size());
	serializer.Serialize((uint32)_materialParams.size());

	//Записать текстуры
	for (auto& _texture : _materialTextures) {
		serializer.Serialize(_texture->_name);
		serializer.Serialize(_texture->_resource.GetId());
	}

	//Записать параметры
	for (auto& _param : _materialParams) {
		serializer.Serialize(_param.name);
		serializer.Serialize(_param.value.GetType());
		serializer.WriteBytes(_param.value.GetValuePtr(), sizeof(MultitypeData));
	}
	
	std::ofstream stream(fpath, std::ios::binary);
	stream.write((const char*)serializer.GetBytes(), serializer.GetSerializedSize());
	stream.close();
}
void Material::Deserialize(byte* data, uint32 size) {
	if (size == 0 || data == nullptr)
		return;

	ByteSolver deserializer(data, size);

	std::string header = deserializer.ReadNextString();

	if (header != "vs_material") {
		Logger::Log() << "Error loading material - incorrect header\n";
		return;
	}

	std::string material_template = deserializer.ReadNextString();

	SetTemplate(material_template);

	uint32 textures_count = deserializer.GetValue<uint32>();
	uint32 params_count = deserializer.GetValue<uint32>();

	for (uint32 texture_i = 0; texture_i < textures_count; texture_i++) {
		std::string name = deserializer.ReadNextString();

		ResourceReference ref;
		ref.SetResource(deserializer.GetGuid());
		SetTexture(name, ref);
	}

	for (uint32 param_i = 0; param_i < params_count; param_i++) {
		std::string name = deserializer.ReadNextString();
		ValueType type = deserializer.GetValue<ValueType>();
		MultitypeData data = deserializer.GetValue<MultitypeData>();
		
		Variant value;
		value.SetData(type, data);

		SetParameter(name, value);
	}
}
