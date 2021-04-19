#include "VulkanPipeline.hpp"
#include "VulkanRAPI.hpp"
#include <vector>

using namespace VSGE;

VkFormat GetVertexLayoutFormatVK(VertexLayoutFormat format) {
	switch (format) {
	case VL_FORMAT_RG32_SFLOAT:
		return VK_FORMAT_R32G32_SFLOAT;
	case VL_FORMAT_RGB32_SFLOAT:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case VL_FORMAT_RGBA32_SFLOAT:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	
	case VL_FORMAT_RG32_SINT:
		return VK_FORMAT_R32G32_SINT;
	case VL_FORMAT_RGB32_SINT:
		return VK_FORMAT_R32G32B32_SINT;
	case VL_FORMAT_RGBA32_SINT:
		return VK_FORMAT_R32G32B32A32_SINT;
	}
	return VK_FORMAT_R32G32B32_SFLOAT;
}

bool VulkanPipeline::Create(VulkanPipelineConf& Conf, VulkanShader& shader, VulkanRenderPass& rpass, VertexLayout& vl, VulkanPipelineLayout& layout) {
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	//Array for shader stage create infos
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo;

	uint32 stages = shader.GetModulesCount();
	for (uint32 stage = 0; stage < stages; stage++) {
		VulkanShaderPair stage_pair = shader.GetModules()[stage];
		VkShaderStageFlagBits stageName = GetShaderStageVK(stage_pair.second);

		VkPipelineShaderStageCreateInfo stageCreateInfo = {};
		stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateInfo.flags = 0;
		stageCreateInfo.pNext = nullptr;
		stageCreateInfo.stage = stageName;
		stageCreateInfo.module = stage_pair.first;
		stageCreateInfo.pName = "main";
		//Push new struct to vector
		shaderStagesInfo.push_back(stageCreateInfo);
	}

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = Conf.primitiveTopology;
	inputAssembly.primitiveRestartEnable = Conf.primitiveRestartEnable;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = Conf.polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = Conf.cullFaceMode;
	rasterizer.frontFace = Conf.frontFace;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//If vulkan width and height are zero
	//then set values from current swapchain
	VulkanSwapChain* swchain = vulkan->GetSwapChain();
	if (Conf.Viewport.width == 0) {
		Conf.Viewport.width = (float)swchain->GetExtent().width;
	}
	if (Conf.Viewport.height == 0) {
		Conf.Viewport.height = (float)swchain->GetExtent().height;
	}

	//Set scissor to cut nothing
	VkRect2D scissor;
	scissor.offset = { 0, 0};
	scissor.extent = { (uint32)Conf.Viewport.width, (uint32)Conf.Viewport.height };

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &Conf.Viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = Conf.DepthTest;
	depthStencil.depthWriteEnable = Conf.DepthTest;
	depthStencil.depthCompareOp = Conf.DepthOp;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = Conf.StencilTest;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = vl.vertexSize;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = 
		static_cast<uint32>(vl.items.size());

	std::vector<VkVertexInputAttributeDescription> attrDescriptions;

	for (auto vertexItem : vl.items) {
		VkVertexInputAttributeDescription descr = {};
		descr.binding = vertexItem.binding;
		descr.location = vertexItem.location;
		descr.offset = vertexItem.offset;
		descr.format = GetVertexLayoutFormatVK(vertexItem.format);
		attrDescriptions.push_back(descr);
	}

	vertexInputInfo.pVertexAttributeDescriptions = attrDescriptions.data();

	VkPipelineColorBlendAttachmentState colorBlendAttachments[6];
	colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments[0].blendEnable = VK_FALSE;
	colorBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	for (int i = 1; i < 6; i++) {
		colorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachments[i].blendEnable = VK_FALSE;
	}

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = colorBlendAttachments;

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_FRONT_FACE_EXT,
		VK_DYNAMIC_STATE_CULL_MODE_EXT
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 0;
	dynamicState.pDynamicStates = dynamicStates;

	VkGraphicsPipelineCreateInfo pipeline_create_info = {};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.pNext = nullptr;
	pipeline_create_info.stageCount = static_cast<uint32>(shaderStagesInfo.size()); //Usually, we have two shader stages (vertex and fragment)
	pipeline_create_info.pStages = shaderStagesInfo.data(); //Store shader stages
	//Now bind all fucking structures, that created above
	pipeline_create_info.pVertexInputState = &vertexInputInfo;
	pipeline_create_info.pInputAssemblyState = &inputAssembly;
	pipeline_create_info.pViewportState = &viewportState;
	pipeline_create_info.pRasterizationState = &rasterizer;
	pipeline_create_info.pMultisampleState = &multisampling;
	pipeline_create_info.pDepthStencilState = &depthStencil;

	pipeline_create_info.pColorBlendState = &colorBlending;
	pipeline_create_info.pDynamicState = &dynamicState;
	pipeline_create_info.layout = layout.GetPipelineLayout();
	//Bind render pass
	pipeline_create_info.renderPass = rpass.GetRenderPass();
	pipeline_create_info.subpass = 0;
	//For future recreation
	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_create_info.basePipelineIndex = -1; // Optional
	//Try to create pipeline
	
	VulkanDevice* device = vulkan->GetDevice();
	if (vkCreateGraphicsPipelines(device->getVkDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &this->mPipeline) != VK_SUCCESS)
		return false;

	mCreated = true;

	return true;
}

void VulkanPipeline::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroyPipeline(device->getVkDevice(), mPipeline, nullptr);
	}
}