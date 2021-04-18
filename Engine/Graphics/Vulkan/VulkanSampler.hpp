#include "../Sampler.hpp"

#include <vulkan/vulkan.hpp>

namespace VSGE {
	class VulkanSampler : public Sampler {
	private:
		VkSampler mSampler;
	public:
		VkSampler GetSampler() { return mSampler; }

		bool Create();

		void Destroy();
	};
}