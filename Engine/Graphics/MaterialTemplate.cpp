#include "Material.hpp"
#include "Shader.hpp"
#include <Core/ByteSerialize.hpp>
#include <Core/ByteSolver.hpp>
#include <fstream>
#include "string.h"

using namespace VSGE;

MaterialTemplate::MaterialTemplate() :
	_shader(nullptr),
	_pipeline(nullptr),
	_cullMode(CULL_MODE_FRONT),
	_depthTest(true),
	_render_stage(RENDER_STAGE_GBUFFER)
{
	SetupDefaultVertexLayout();
}

MaterialTemplate::~MaterialTemplate() {
	SAFE_RELEASE(_pipeline);
}

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

const std::string& MaterialTemplate::GetName() {
	return _name;
}

void MaterialTemplate::SetName(const std::string& name) {
	_name = name;
}

void MaterialTemplate::SetPipeline(GraphicsPipeline* pipeline) {
	_pipeline = pipeline;
}

GraphicsPipeline* MaterialTemplate::GetPipeline() {
	return _pipeline;
}

tMaterialTexturesList& MaterialTemplate::GetTextures() {
	return _materialTextures;
}

tMaterialParamsList& MaterialTemplate::GetParams() {
	return _materialParams;
}

Shader* MaterialTemplate::GetShader() {
	return _shader;
}

void MaterialTemplate::SetShader(const std::string& shader_name) {
	_shader = ShaderCache::Get()->GetShader(shader_name);
}

void MaterialTemplate::SetShader(Shader* shader) {
	_shader = shader;
}

CullMode MaterialTemplate::GetCullMode() {
	return _cullMode;
}

void MaterialTemplate::SetCullMode(CullMode mode) {
	_cullMode = mode;
}

bool MaterialTemplate::GetDepthTest(){
	return _depthTest;
}

void MaterialTemplate::SetDepthTest(bool depth_test){
	_depthTest = depth_test;
}

void MaterialTemplate::SetRenderStage(MaterialRenderStage stage) {
	_render_stage = stage;
}

MaterialRenderStage MaterialTemplate::GetRenderStage() {
	return _render_stage;
}

void MaterialTemplate::SetVertexLayout(const VertexLayout& vertexLayout) {
	_vertexLayout = vertexLayout;
}

void MaterialTemplate::SetBlendingAttachmentDesc(uint32 attachment, const BlendAttachmentDesc& desc) {
	_blendDescs.insert(std::pair(attachment, desc));
}

void MaterialTemplate::AddParameter(const std::string& name, Variant baseValue) {
	MaterialParameter param;
	param.name = name;
	param.value = baseValue;
	_materialParams.push_back(param);
}

void MaterialTemplate::AddTexture(const std::string& name, uint32 binding) {
	MaterialTexture* texture = new MaterialTexture;
	texture->_name = name;
	texture->_binding = binding;
	_materialTextures.push_back(texture);

	AddParameter("@has_" + name, false);
}