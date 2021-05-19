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
		if (texture.name == texture_name)
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
	GetTextureByName(texture_name)->resource = *texture;
	CopyParamsToBuffer();
	_texturesDirty = true;
}
void Material::SetParameter(const std::string& parameter_name, MultitypeValue value) {
	GetParameterByName(parameter_name)->value = value;
	CopyParamsToBuffer();
	_paramsDirty = true;
}

void Material::CopyParamsToBuffer() {

}