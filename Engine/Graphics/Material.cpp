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