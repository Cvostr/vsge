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
	m_materialMapsSampler(nullptr)
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

	//������ ������� ��������
	m_emptyZeroTexture = new VulkanTexture;
	m_emptyZeroTexture->Create(2, 2);
	m_emptyZeroTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, 0);
	m_emptyZeroTexture->SetReadyToUseInShaders();
	
	//������ ���������� ��������
	m_emptyZeroCubeTexture = new VulkanTexture;
	m_emptyZeroCubeTexture->SetCubemap(true);
	m_emptyZeroCubeTexture->Create(2, 2, FORMAT_RGBA, 6, 1);
	for (uint32 i = 0; i < 6; i++)
		m_emptyZeroCubeTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, i);
	m_emptyZeroCubeTexture->SetReadyToUseInShaders();

	//������ �������� - ������
	m_emptyZero2dArrayTexture = new VulkanTexture;
	m_emptyZero2dArrayTexture->Create(2, 2, FORMAT_RGBA, 6, 2);
	for (uint32 i = 0; i < 2; i++)
		m_emptyZero2dArrayTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, i);
	m_emptyZero2dArrayTexture->SetReadyToUseInShaders();

	memset(tempTextureData, 255, 16);
	
	//����� ������� ��������
	m_emptyOneTexture = new VulkanTexture;
	m_emptyOneTexture->Create(2, 2);
	m_emptyOneTexture->AddMipLevel((byte*)tempTextureData, 16, 2, 2, 0, 0);
	m_emptyOneTexture->SetReadyToUseInShaders();

	//����� ���������� ��������
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