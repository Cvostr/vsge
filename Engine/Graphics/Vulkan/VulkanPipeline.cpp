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

	case VL_FORMAT_R32_UINT:
		return VK_FORMAT_R32_UINT;
	case VL_FORMAT_RG32_UINT:
		return VK_FORMAT_R32G32_UINT;
	case VL_FORMAT_RGB32_UINT:
		return VK_FORMAT_R32G32B32_UINT;
	case VL_FORMAT_RGBA32_UINT:
		return VK_FORMAT_R32G32B32A32_UINT;
	}
	return VK_FORMAT_R32G32B32_SFLOAT;
}

bool VulkanPipeline::Create(VulkanShader* shader, VulkanRenderPass* rpass, VertexLayout& vl, VulkanPipelineLayout* layout) {
	_pipelineLayout = layout;
	VulkanRAPI* vulkan = VulkanRAPI::Get();
	//Array for shader stage create infos
	std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo;

	uint32 stages = shader->GetModulesCount();
	for (uint32 stage = 0; stage < stages; stage++) {
		VulkanShaderPair stage_pair = shader->GetModules()[stage];
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
	inputAssembly.topology = (VkPrimitiveTopology)_primitiveTopology;
	inputAssembly.primitiveRestartEnable = _primitiveRestartEnable;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = (VkPolygonMode)_polygonMode;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = _cullMode;
	rasterizer.frontFace = (VkFrontFace)_frontFace;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	//If vulkan width and height are zero
	//then set values from current swapchain
	VulkanSwapChain* swchain = vulkan->GetSwapChain();

	VkViewport Viewport;
	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = (float)swchain->GetExtent().width;
	Viewport.height = (float)swchain->GetExtent().height;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;
	VkRect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { swchain->GetExtent().width, swchain->GetExtent().height };

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &Viewport;
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
	depthStencil.depthTestEnable = _depthTest;
	depthStencil.depthWriteEnable = _depthTest;
	depthStencil.depthCompareOp = (VkCompareOp)_depthOp;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = _stencilTest;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	std::vector<VkVertexInputBindingDescription> vertexInputBindingDescrs;

	for (uint32 vertexBinding = 0; vertexBinding < vl.vertexBindingsCount; vertexBinding ++) {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = vertexBinding;
		bindingDescription.stride = vl.vertexSizes[vertexBinding];
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		vertexInputBindingDescrs.push_back(bindingDescription);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32>(vertexInputBindingDescrs.size());
	vertexInputInfo.pVertexBindingDescriptions = vertexInputBindingDescrs.data(); // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32>(vl.items.size());

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

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

	for (uint32 attachment_i = 0; attachment_i < rpass->GetColorAttachmentsCount(); attachment_i++) {
		bool blend_enable = false;
		uint32 color_writemask = COLOR_R | COLOR_G | COLOR_B | COLOR_A;
		BlendFactor srcColor = BLEND_FACTOR_ONE;
		BlendFactor dstColor = BLEND_FACTOR_ZERO;

		if (_blendDescs.count(attachment_i) == 1) {
			BlendAttachmentDesc desc = _blendDescs.at(attachment_i);
			blend_enable = desc._blending;
			color_writemask = desc._colorMask;
			srcColor = desc._srcColor;
			dstColor = desc._dstColor;
		}

		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.colorWriteMask = color_writemask;
		colorBlendAttachment.blendEnable = blend_enable;
		colorBlendAttachment.srcColorBlendFactor = (VkBlendFactor)srcColor;
		colorBlendAttachment.dstColorBlendFactor = (VkBlendFactor)dstColor;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		colorBlendAttachments.push_back(colorBlendAttachment);
	}

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.attachmentCount = rpass->GetColorAttachmentsCount();
	colorBlending.pAttachments = colorBlendAttachments.data();

	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH,
		VK_DYNAMIC_STATE_FRONT_FACE_EXT,
		VK_DYNAMIC_STATE_CULL_MODE_EXT
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 3;
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
	pipeline_create_info.layout = layout->GetPipelineLayout();
	//Bind render pass
	pipeline_create_info.renderPass = rpass->GetRenderPass();
	pipeline_create_info.subpass = 0;
	//For future recreation
	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipeline_create_info.basePipelineIndex = -1; // Optional
	//Try to create pipeline
	
	VulkanDevice* device = vulkan->GetDevice();
	if (vkCreateGraphicsPipelines(device->getVkDevice(), VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &_pipeline) != VK_SUCCESS)
		return false;

	mCreated = true;

	return true;
}

void VulkanPipeline::Destroy() {
	if (mCreated) {
		VulkanRAPI* vulkan = VulkanRAPI::Get();
		VulkanDevice* device = vulkan->GetDevice();
		vkDestroyPipeline(device->getVkDevice(), _pipeline, nullptr);
	}
}