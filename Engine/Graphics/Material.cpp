#include "Material.hpp"
#include "Shader.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>

using namespace VSGE;

MaterialTemplateCache* MaterialTemplateCache::_this = nullptr;

MaterialTemplateCache cache;

void MaterialTemplate::SetupDefaultVertexLayout() {
	_vertexLayout.AddBinding(sizeof(Vertex));
	_vertexLayout.AddItem(0, offsetof(Vertex, pos), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(1, offsetof(Vertex, uv), VertexLayoutFormat::VL_FORMAT_RG32_SFLOAT);
	_vertexLayout.AddItem(2, offsetof(Vertex, normal), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(3, offsetof(Vertex, tangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);
	_vertexLayout.AddItem(4, offsetof(Vertex, bitangent), VertexLayoutFormat::VL_FORMAT_RGB32_SFLOAT);

	_vertexLayout.AddBinding(sizeof(VertexSkinningData));
	_vertexLayout.AddItem(5, offsetof(VertexSkinningData, ids), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	_vertexLayout.AddItem(6, offsetof(VertexSkinningData, ids[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);
	_vertexLayout.AddItem(7, offsetof(VertexSkinningData, ids[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SINT, 1);

	_vertexLayout.AddItem(8, offsetof(VertexSkinningData, weights), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	_vertexLayout.AddItem(9, offsetof(VertexSkinningData, weights[4]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);
	_vertexLayout.AddItem(10, offsetof(VertexSkinningData, weights[8]), VertexLayoutFormat::VL_FORMAT_RGBA32_SFLOAT, 1);

	_vertexLayout.AddItem(11, offsetof(VertexSkinningData, bones_num), VertexLayoutFormat::VL_FORMAT_R32_UINT, 1);
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
	_materialTextures.push_back(texture);

	AddParameter("@has_" + name, false);
}

MaterialTemplate* MaterialTemplateCache::GetTemplate(const std::string& name) {
	for (auto _template : _templates) {
		if (_template->GetName() == name) {
			return _template;
		}
	}
	return nullptr;
}

void Material::SetTemplate(MaterialTemplate* mat_template) {
	if (mat_template == nullptr)
		return;

	this->_template = mat_template;

	for (auto texture : mat_template->GetTextures()) {
		this->_materialTextures.push_back(texture);
	}
	for (auto param : mat_template->GetParams()) {
		this->_materialParams.push_back(param);
	}
}

void Material::SetTemplate(const std::string& template_name) {
	SetTemplate(MaterialTemplateCache::Get()->GetTemplate(template_name));
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

void Material::SetTexture(const std::string& texture_name, ResourceReference& texture) {
	GetTextureByName(texture_name)->_resource = texture;
	if(texture.GetResource() != nullptr)
		SetParameter("@has_" + texture_name, true);
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
		if (param.value.GetType() == VALUE_TYPE_BOOL) {
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
			writtenBytes += 4;
		}
		if (param.value.GetType() == VALUE_TYPE_INT) {
			writtenBytes += 4;
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
		}
		if (param.value.GetType() == VALUE_TYPE_FLOAT) {
			writtenBytes += 4;
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 4);
		}
		if (param.value.GetType() == VALUE_TYPE_VEC3F) {
			writtenBytes = align(writtenBytes);
			memcpy(bytes + writtenBytes, param.value.GetValuePtr(), 12);
		}
	}

	return writtenBytes;
}

void Material::Serialize(const std::string& fpath) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Material";
	out << YAML::Key << "template" << YAML::Value << GetTemplate()->GetName();
	
	out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
	for (auto& texture : this->_materialTextures) {
		out << YAML::BeginMap;
		out << YAML::Key << "Texture" << YAML::Value << texture._name;
		out << YAML::Key << "res" << YAML::Value << texture._resource.GetResourceName();
		out << YAML::EndMap; // Entity
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Params" << YAML::Value << YAML::BeginSeq;
	for (auto& param : this->_materialParams) {
		out << YAML::BeginMap;
		out << YAML::Key << "Param" << YAML::Value << param.name;
		out << YAML::Key << "type" << YAML::Value << param.value.GetType();
		out << YAML::EndMap; // Entity
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(fpath);
	fout << out.c_str();
}
void Material::Deserialize(const std::string& fpath) {

}