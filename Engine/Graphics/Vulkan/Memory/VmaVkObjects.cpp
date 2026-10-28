#include "VulkanMA.hpp"

using namespace VSGE;

VmaVkBuffer::VmaVkBuffer() :
	Buffer(VK_NULL_HANDLE),
	_allocation(nullptr)
{
}

VmaVkBuffer::VmaVkBuffer(VmaVkBuffer&& old) noexcept :
	Buffer(old.Buffer),
	_allocation(old._allocation)
{
	old.Buffer = nullptr;
	old._allocation = nullptr;
}

VmaVkImage::VmaVkImage() :
	Image(VK_NULL_HANDLE),
	_allocation(nullptr)
{
}

VmaVkImage::VmaVkImage(VkImage image, void* allocation) :
	Image(image),
	_allocation(allocation)
{
}

VmaVkImage::VmaVkImage(VmaVkImage&& old) noexcept :
	Image(old.Image),
	_allocation(old._allocation)
{
	old.Image = nullptr;
	old._allocation = nullptr;
}