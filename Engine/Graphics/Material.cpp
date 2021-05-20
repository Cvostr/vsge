#include "Material.hpp"
#include "Shader.hpp"

using namespace VSGE;

void MaterialTemplate::SetupDefaultVertexLayout() {
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	_vertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
}

void MaterialTemplate::SetShader(const std::string& shader_name) {
	_shader = ShaderCache::Get()->GetShader(shader_name);
}

void MaterialTemplate::AddParameter(const std::string& name, MultitypeValue baseValue) {
	MaterialParameter param;
	param.name = name;
	param.value = baseValue;
	_materialParams.push_back(param);
}

void MaterialTemplate::AddTexture(const std::string& name, uint32 binding) {
	MaterialTexture texture;
	texture._name = name;
	texture._binding = binding;
}

void Material::SetTemplate(MaterialTemplate* mat_template) {
	for (auto texture : mat_template->GetTextures()) {
		this->_materialTextures.push_back(texture);
	}
	for (auto param : mat_template->GetParams()) {
		this->_materialParams.push_back(param);
	}
}

MaterialTexture* Material::GetTextureByName(const std::string& texture_name) {
	for (MaterialTexture& texture : _materialTextures) {
		if (texture._name == texture_name)
			return &texture;
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

void Material::SetTexture(const std::string& texture_name, ResourceReference* texture) {
	GetTextureByName(texture_name)->_resource = *texture;
	_texturesDirty = true;
}
void Material::SetParameter(const std::string& parameter_name, MultitypeValue value) {
	GetParameterByName(parameter_name)->value = value;
	_paramsDirty = true;
}

uint32 align(uint32 in) {
	return in + (16 - (in % 16));
}

uint32 Material::CopyParamsToBuffer(char** out) {
	*out = new char[1024];
	char* bytes = *out;
	uint32 writtenBytes = 0;

	for (MaterialParameter& param : _materialParams) {
		
		if (param.value.GetType() == VALUE_TYPE_INT) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
		}
		if (param.value.GetType() == VALUE_TYPE_FLOAT) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
		}
		if (param.value.GetType() == VALUE_TYPE_VEC3F) {
			writtenBytes = align(writtenBytes);
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 12);
		}
	}

	return writtenBytes;
}