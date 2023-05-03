#include "VulkanRenderingGenerics.hpp"

using namespace VSGE;

VulkanRenderingGenerics* VulkanRenderingGenerics::_this = new VulkanRenderingGenerics;

VulkanRenderingGenerics::VulkanRenderingGenerics() :
	m_initialized(false),
	m_emptyZeroTexture(nullptr),
	m_emptyZeroCubeTexture(nullptr),
	m_emptyZero2dArrayTexture(nullptr),
	m_emptyOneTexture(nullptr),
	m_emptyOneCubeTexture(nullptr),
	m_samplerIBL(nullptr),
	m_materialMapsSampler(nullptr),
	m_attachmentSampler(nullptr),
	m_screenMesh(nullptr)
{
	_this = this;
}

VulkanRenderingGenerics* VulkanRenderingGenerics::Get() {
	return _this;
}

bool VulkanRenderingGenerics::Init() {
	if (m_initialized)
		return true;

	//---------------------Textures------------------

	char* tempTextureData = new char[2 * 2 * 4];
	memset(tempTextureData, 0, 16);

	//Черная плоская текстура
	m_emptyZeroTexture = new VulkanTexture;
	m_emptyZeroTexture->Create(2, 2);
	m_emptyZeroTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, 0);
	m_emptyZeroTexture->SetReadyToUseInShaders();
	
	//Черная кубическая текстура
	m_emptyZeroCubeTexture = new VulkanTexture;
	m_emptyZeroCubeTexture->SetCubemap(true);
	m_emptyZeroCubeTexture->Create(2, 2, FORMAT_RGBA, 6, 1);
	for (uint32 i = 0; i < 6; i++)
		m_emptyZeroCubeTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, i);
	m_emptyZeroCubeTexture->SetReadyToUseInShaders();

	//Черная текстура - массив
	m_emptyZero2dArrayTexture = new VulkanTexture;
	m_emptyZero2dArrayTexture->Create(2, 2, FORMAT_RGBA, 6, 2);
	for (uint32 i = 0; i < 2; i++)
		m_emptyZero2dArrayTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, i);
	m_emptyZero2dArrayTexture->SetReadyToUseInShaders();

	memset(tempTextureData, 255, 16);
	
	//Белая плоская текстура
	m_emptyOneTexture = new VulkanTexture;
	m_emptyOneTexture->Create(2, 2);
	m_emptyOneTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, 0);
	m_emptyOneTexture->SetReadyToUseInShaders();

	//Белая кубическая текстура
	m_emptyOneCubeTexture = new VulkanTexture;
	m_emptyOneCubeTexture->SetCubemap(true);
	m_emptyOneCubeTexture->Create(2, 2, FORMAT_RGBA, 6, 1);
	for (uint32 i = 0; i < 6; i++)
		m_emptyOneCubeTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, i);
	m_emptyOneCubeTexture->SetReadyToUseInShaders();

	delete[] tempTextureData;

	//---------------------Samplers------------------

	m_materialMapsSampler = new VulkanSampler;
	m_materialMapsSampler->SetWrapModes(SAMPLER_WRAP_REPEAT, SAMPLER_WRAP_REPEAT);
	m_materialMapsSampler->SetLodsRanges(0, 10);
	m_materialMapsSampler->Create();

	m_samplerIBL = new VulkanSampler;
	m_samplerIBL->SetLodsRanges(-1000, 1000);
	m_samplerIBL->Create();

	m_attachmentSampler = new VulkanSampler;
	m_attachmentSampler->SetFilteringModes(SAMPLER_FILTERING_NEAREST, SAMPLER_FILTERING_NEAREST);
	m_attachmentSampler->Create();

	//---------------------Meshes----------------------------------
	m_screenMesh = new VulkanMesh;
	Vertex plane_verts[] = {
		// positions              // texture coords
		Vertex(Vec3(1.0f,  1.0f, 0.0f),   Vec2(1.0f, 1.0f),   Vec3(0, 0, 1)),   // top right
		Vertex(Vec3(1.0f, -1.0f, 0.0f),   Vec2(1.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom right
		Vertex(Vec3(-1.0f, -1.0f, 0.0f),  Vec2(0.0f, 0.0f),   Vec3(0, 0, 1)),   // bottom left
		Vertex(Vec3(-1.0f,  1.0f, 0.0f),  Vec2(0.0f, 1.0f),   Vec3(0, 0, 1))   // top left
	};

	unsigned int plane_inds[] = { 0,1,2, 0,2,3 };

	m_screenMesh->SetVertexBuffer(plane_verts, 4);
	m_screenMesh->SetIndexBuffer(plane_inds, 6);
	m_screenMesh->Create();

	//------------------BRDF LUT ------------------------------------

	m_brdf_lut = new Vulkan_BRDF_LUT;
	m_brdf_lut->Create();	

	//--------------------Shaders-----------------------
	ShaderStorage::Get()->LoadShaderBundle("../Shaders/shaders.bundle", "../Shaders/shaders.map");
	VulkanShader* pbr = new VulkanShader;
	pbr->AddShaderFromFile("pbr.vert", SHADER_STAGE_VERTEX);
	pbr->AddShaderFromFile("pbr.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(pbr, "PBR");

	VulkanShader* vegetable = new VulkanShader;
	vegetable->AddShaderFromFile("vegetable.vert", SHADER_STAGE_VERTEX);
	vegetable->AddShaderFromFile("vegetable.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(vegetable, "Vegetable");

	VulkanShader* deferred_light = new VulkanShader;
	deferred_light->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	deferred_light->AddShaderFromFile("deferred_pbr.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(deferred_light, "Deferred");

	VulkanShader* deferred_light_envmap = new VulkanShader;
	deferred_light_envmap->AddShaderFromFile("postprocess.vert", SHADER_STAGE_VERTEX);
	deferred_light_envmap->AddShaderFromFile("deferred_pbr_envmap.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(deferred_light_envmap, "Deferred_envmap");

	VulkanShader* particle = new VulkanShader;
	particle->AddShaderFromFile("particle.vert", SHADER_STAGE_VERTEX);
	particle->AddShaderFromFile("particle.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(particle, "Particle");

	VulkanShader* skybox = new VulkanShader;
	skybox->AddShaderFromFile("skybox.vert", SHADER_STAGE_VERTEX);
	skybox->AddShaderFromFile("skybox.frag", SHADER_STAGE_FRAGMENT);
	ShaderCache::Get()->AddShader(skybox, "Skybox");

	m_initialized = true;
	return false;
}

VulkanTexture* VulkanRenderingGenerics::GetBlackTexture() {
	return m_emptyZeroTexture;
}

VulkanTexture* VulkanRenderingGenerics::GetWhiteTexture() {
	return m_emptyOneTexture;
}

VulkanTexture* VulkanRenderingGenerics::GetBlackCubeTexture() {
	return m_emptyZeroCubeTexture;
}

VulkanTexture* VulkanRenderingGenerics::GetWhiteCubeTexture() {
	return m_emptyOneCubeTexture;
}

VulkanTexture* VulkanRenderingGenerics::GetBlack2dArrayTexture() {
	return m_emptyZero2dArrayTexture;
}

VulkanSampler* VulkanRenderingGenerics::GetSpecularIBLSampler() {
	return m_samplerIBL;
}

VulkanSampler* VulkanRenderingGenerics::GetMaterialMapsSampler() {
	return m_materialMapsSampler;
}

VulkanMesh* VulkanRenderingGenerics::GetScreenMesh() {
	return m_screenMesh;
}

VulkanSampler* VulkanRenderingGenerics::GetAttachmentSampler() {
	return m_attachmentSampler;
}

Vulkan_BRDF_LUT* VulkanRenderingGenerics::GetBRDF() {
	return m_brdf_lut;
}