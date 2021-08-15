#include "Material.hpp"
#include "Shader.hpp"
#include <Core/ByteSerialize.hpp>
#include <Core/ByteSolver.hpp>
#include <Core/Logger.hpp>
#include <fstream>
#include "string.h"

using namespace VSGE;

MaterialTemplateCache* MaterialTemplateCache::_this = nullptr;

MaterialTemplateCache template_cache;

MaterialTemplate::MaterialTemplate() :
	_shader(nullptr),
	_pipeline(nullptr),
	_cullMode(CULL_MODE_FRONT),
	_depthTest(true)
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

void MaterialTemplate::SetVertexLayout(const VertexLayout& vertexLayout) {
	_vertexLayout = vertexLayout;
}

void MaterialTemplate::SetBlendingAttachmentDesc(uint32 attachment, const BlendAttachmentDesc& desc) {
	_blendDescs.insert(std::pair(attachment, desc));
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

Material::Material() :
			_template(nullptr),
			_paramsDirty(true),
			_texturesDirty(true),
			_descriptors(nullptr)
		{}

Material::~Material() {
	SAFE_RELEASE(_descriptors);
}

void Material::SetTemplate(MaterialTemplate* mat_template) {
	if (mat_template == nullptr)
		return;

	this->_template = mat_template;

	//clear params and textures first
	_materialTextures.clear();
	_materialParams.clear();

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
	bool hasTexture = texture.GetResource() != nullptr;
	SetParameter("@has_" + texture_name, hasTexture);
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
	SAFE_RELEASE(_descriptors);
}

void Material::Serialize(const std::string& fpath) {
	ByteSerialize serializer;
	//write header without terminator
	serializer.WriteBytes("vs_material", 12);

	std::string template_name = "";
	if (GetTemplate()) {
		template_name = GetTemplate()->GetName();
	}
	//write name of template
	serializer.Serialize(template_name);
	//Write count of textures and params
	serializer.Serialize((uint32)_materialTextures.size());
	serializer.Serialize((uint32)_materialParams.size());
	//Write textures
	for (auto& _texture : _materialTextures) {
		serializer.Serialize(_texture._name);
		serializer.Serialize(_texture._resource.GetResourceName());
	}

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
		std::string texture_name = deserializer.ReadNextString();

		ResourceReference ref;
		ref.SetResource(texture_name);
		SetTexture(name, ref);
	}

	for (uint32 param_i = 0; param_i < params_count; param_i++) {
		std::string name = deserializer.ReadNextString();
		ValueType type = deserializer.GetValue<ValueType>();
		MultitypeData data = deserializer.GetValue<MultitypeData>();
		
		MultitypeValue value;
		value.SetData(type, data);

		SetParameter(name, value);
	}
}
