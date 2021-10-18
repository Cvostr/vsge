#include "VulkanPostprocessing.hpp"
#include <Graphics/Vulkan/VulkanRAPI.hpp>

using namespace VSGE;

VulkanPostprocessing::VulkanPostprocessing() {
	_input_texture = nullptr;
	_input_depth = nullptr;
	_input_normals = nullptr;
	_input_positions = nullptr;
}
VulkanPostprocessing::~VulkanPostprocessing() {

}

void VulkanPostprocessing::SetInputTextures(
	Texture* input,
	Texture* depth,
	Texture* normals,
	Texture* positions)
{
	this->_input_texture = input;
	this->_input_depth = depth;
	this->_input_normals = normals;
	this->_input_positions = positions;
}

void VulkanPostprocessing::Create() {
	_gamma_correction = new VulkanGammaCorrection;
	_gamma_correction->Create();

	_bloom = new VulkanBloom;
	_bloom->Create();

	_ssao = new VulkanSSAO;
	_ssao->Create();

	_cmdpool = new VulkanCommandPool;
	_cmdpool->Create(VulkanRAPI::Get()->GetDevice()->GetComputeQueueFamilyIndex());

	_cmdbuf = new VulkanCommandBuffer;
	_cmdbuf->Create(_cmdpool);

	_begin_semaphore = new VulkanSemaphore;
	_begin_semaphore->Create();
}
void VulkanPostprocessing::Destroy() {
	SAFE_RELEASE(_begin_semaphore)
	SAFE_RELEASE(_cmdbuf);
	SAFE_RELEASE(_cmdpool);

	SAFE_RELEASE(_gamma_correction)
}

void VulkanPostprocessing::FillCommandBuffer() {
	_gamma_correction->SetInputTexture(_input_texture);

	_cmdbuf->Begin();
	_gamma_correction->FillCommandBuffer(_cmdbuf);
	_cmdbuf->End();
}

VulkanTexture* VulkanPostprocessing::GetOutputTexture() {
	return (VulkanTexture*)_gamma_correction->GetOutputTexture();
}
VulkanSemaphore* VulkanPostprocessing::GetBeginSemaphore() {
	return _begin_semaphore;
}
void VulkanPostprocessing::Execute(VulkanSemaphore* end_semaphore) {
	VulkanGraphicsSubmit(*_cmdbuf, *_begin_semaphore, *end_semaphore);
}
void VulkanPostprocessing::ResizeOutput(const Vec2i& new_size) {
	if (_output_sizes == new_size)
		return;

	_output_sizes = new_size;

	_gamma_correction->ResizeOutput(new_size);
}