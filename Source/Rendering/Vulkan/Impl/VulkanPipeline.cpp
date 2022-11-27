#include "Rendering/Vulkan/Impl/VulkanPipeline.h"

namespace Rendering
{
	VulkanPipeline::VulkanPipeline(Utility::HeapAllocator& heapAllocator_,	     Utility::StackAllocator& stackAllocator_,
								   VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanSwapChain&		  vulkanSwapChain_) :
								   heapAllocator(heapAllocator_),
								   stackAllocator(stackAllocator_),
								   vulkanLogicalDevice(vulkanLogicalDevice_),
								   vulkanSwapChain(vulkanSwapChain_),
								   vulkanShaders{ vulkanLogicalDevice, vulkanLogicalDevice, vulkanLogicalDevice }
	{
		wasPipelineCreated   = false;
		wasRenderPassCreated = false;
	}


	VulkanPipeline::~VulkanPipeline()
	{
		Destroy();
	}


	void VulkanPipeline::Destroy()
	{
		vulkanLogicalDevice.WaitFor();

		if (wasPipelineCreated)
		{
			vkDestroyPipelineLayout(vulkanLogicalDevice.GetVkLogicalDevice(), vulkPipelineLayout, nullptr);
			vkDestroyPipeline(vulkanLogicalDevice.GetVkLogicalDevice(), vulkPipeline, nullptr);
		}

		if (wasRenderPassCreated)
		{
			vkDestroyRenderPass(vulkanLogicalDevice.GetVkLogicalDevice(), vulkRenderPass, nullptr);
		}

		wasPipelineCreated   = false;
		wasRenderPassCreated = false;
	}


	bool VulkanPipeline::LoadShaderStage(const char* fName_, const VulkanShader::Type type_)
	{
		// Don't want to be re-initing shaders
		if (type_ > VulkanShader::Num || vulkanShaders[type_].IsValid())
		{
			return false;
		}

		return vulkanShaders[type_].Load(stackAllocator, type_, fName_);
	}


	bool VulkanPipeline::LoadPipeline()
	{
		u8 nTypes = 0;
		VulkanShader::Type typesToUse[VulkanShader::Num];

		// Determine valid shader types and send on to be made as a pipeline
		for (u8 i = 0; i < VulkanShader::Num; ++i)
		{
			if (vulkanShaders[i].IsValid())
			{
				typesToUse[nTypes] = (VulkanShader::Type)i;
				nTypes++;
			}
		}

		// Only create pipeline if we were successful with render pass
		wasRenderPassCreated = CreateRenderPass();
		if (wasRenderPassCreated)
		{
			wasPipelineCreated = CreatePipeline(nTypes, typesToUse);
		}

		return wasRenderPassCreated && wasPipelineCreated;
	}


	bool VulkanPipeline::GetVkPipeline(VkPipeline& vulkPipeline_)
	{
		vulkPipeline_ = vulkPipeline;
		return wasPipelineCreated;
	}


	bool VulkanPipeline::GetVkRenderPass(VkRenderPass& vulkRenderPass_)
	{
		vulkRenderPass_ = vulkRenderPass;
		return wasRenderPassCreated;
	}


	bool VulkanPipeline::CreateRenderPass()
	{
		const VkFormat vulkFormat = vulkanSwapChain.GetVkFormat();

		VkAttachmentDescription colourAttachment = {};
		colourAttachment.format					 = vulkFormat;
		colourAttachment.samples				 = VK_SAMPLE_COUNT_1_BIT;
		colourAttachment.loadOp					 = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colourAttachment.storeOp				 = VK_ATTACHMENT_STORE_OP_STORE;
		colourAttachment.stencilLoadOp			 = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colourAttachment.stencilStoreOp			 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colourAttachment.initialLayout			 = VK_IMAGE_LAYOUT_UNDEFINED;
		colourAttachment.finalLayout			 = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colourAttachmentRef = {};
		colourAttachmentRef.attachment			  = 0;
		colourAttachmentRef.layout				  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDesc = {};
		subpassDesc.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDesc.colorAttachmentCount = 1;
		subpassDesc.pColorAttachments    = &colourAttachmentRef;

		VkSubpassDependency depInfo = {};
		depInfo.srcSubpass			= VK_SUBPASS_EXTERNAL;
		depInfo.dstSubpass			= 0;
		depInfo.srcStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		depInfo.srcAccessMask		= 0;
		depInfo.dstStageMask		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		depInfo.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType				  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount		  = 1;
		createInfo.pAttachments			  = &colourAttachment;
		createInfo.subpassCount			  = 1;
		createInfo.pSubpasses			  = &subpassDesc;
		createInfo.dependencyCount		  = 1;
		createInfo.pDependencies		  = &depInfo;

		return VULK_CHECK_SUCCESS(vkCreateRenderPass, vulkanLogicalDevice.GetVkLogicalDevice(), &createInfo, nullptr, &vulkRenderPass);
	}


	bool VulkanPipeline::CreatePipeline(const u8 nTypes_, VulkanShader::Type* typesToUse_)
	{
		// Create structs for each shader we want to make use of
		VkPipelineShaderStageCreateInfo shaderStageInfo[VulkanShader::Num];
		for (u8 i = 0; i < nTypes_; ++i)
		{
			VkPipelineShaderStageCreateInfo& createInfo = shaderStageInfo[i];
			createInfo								    = {};
			createInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.stage							= Consts::vkShaderStages[typesToUse_[i]];
			createInfo.module							= vulkanShaders[typesToUse_[i]].GetVkShaderModule();
			createInfo.pName							= Consts::shaderMainName;
			createInfo.pSpecializationInfo				= nullptr;
		}

		VkPipelineVertexInputStateCreateInfo vertexStageInfo = {};
		vertexStageInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexStageInfo.vertexBindingDescriptionCount		 = 0;
		vertexStageInfo.pVertexBindingDescriptions			 = nullptr;
		vertexStageInfo.vertexAttributeDescriptionCount		 = 0;
		vertexStageInfo.pVertexAttributeDescriptions		 = nullptr;

		VkPipelineInputAssemblyStateCreateInfo assemblyStageInfo = {};
		assemblyStageInfo.sType								     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyStageInfo.topology								 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyStageInfo.primitiveRestartEnable			     = VK_FALSE;

		const VkExtent2D& vulkExtent = vulkanSwapChain.GetVkExtent();
		VkViewport vulkViewport		 = {};
		vulkViewport.x			     = 0.0f;
		vulkViewport.y				 = 0.0f;
		vulkViewport.width			 = vulkExtent.width;
		vulkViewport.height			 = vulkExtent.height;
		vulkViewport.minDepth		 = 0.0f;
		vulkViewport.maxDepth		 = 1.0f;

		VkRect2D vulkScissor = {};
		vulkScissor.extent   = vulkExtent;
		vulkScissor.offset.x = 0;
		vulkScissor.offset.y = 0;

		VkPipelineViewportStateCreateInfo viewportStageInfo = {};
		viewportStageInfo.sType							    = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStageInfo.viewportCount						= 1;
		viewportStageInfo.pViewports						= &vulkViewport;
		viewportStageInfo.scissorCount						= 1;
		viewportStageInfo.pScissors							= &vulkScissor;

		VkPipelineRasterizationStateCreateInfo rasterStageInfo = {};
		rasterStageInfo.sType								   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterStageInfo.depthClampEnable					   = VK_FALSE;
		rasterStageInfo.rasterizerDiscardEnable				   = VK_FALSE;
		rasterStageInfo.polygonMode							   = VK_POLYGON_MODE_FILL;
		rasterStageInfo.lineWidth							   = 1.0f;
		rasterStageInfo.cullMode							   = VK_CULL_MODE_BACK_BIT;
		rasterStageInfo.frontFace							   = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterStageInfo.depthBiasEnable						   = VK_FALSE;
		rasterStageInfo.depthBiasConstantFactor				   = 0.0f;
		rasterStageInfo.depthBiasClamp						   = 0.0f;
		rasterStageInfo.depthBiasSlopeFactor				   = 0.0f;

		VkPipelineMultisampleStateCreateInfo msaaStageInfo = {};
		msaaStageInfo.sType								   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		msaaStageInfo.sampleShadingEnable				   = VK_FALSE;
		msaaStageInfo.rasterizationSamples				   = VK_SAMPLE_COUNT_1_BIT;
		msaaStageInfo.minSampleShading				       = 1.0f;
		msaaStageInfo.pSampleMask						   = nullptr;
		msaaStageInfo.alphaToCoverageEnable				   = VK_FALSE;
		msaaStageInfo.alphaToOneEnable					   = VK_FALSE;

		VkPipelineDepthStencilStateCreateInfo depthStageInfo = {};
		depthStageInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStageInfo.depthTestEnable						 = VK_FALSE;
		depthStageInfo.depthWriteEnable					     = VK_FALSE;
		depthStageInfo.depthCompareOp						 = VK_COMPARE_OP_LESS;
		depthStageInfo.depthBoundsTestEnable			     = VK_FALSE;
		depthStageInfo.stencilTestEnable				     = VK_FALSE;

		VkPipelineColorBlendAttachmentState blendStageInfo = {};
		blendStageInfo.blendEnable						   = VK_TRUE;
		blendStageInfo.srcColorBlendFactor				   = VK_BLEND_FACTOR_SRC_ALPHA;
		blendStageInfo.dstColorBlendFactor				   = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blendStageInfo.colorBlendOp						   = VK_BLEND_OP_ADD;
		blendStageInfo.srcAlphaBlendFactor				   = VK_BLEND_FACTOR_ONE;
		blendStageInfo.dstAlphaBlendFactor				   = VK_BLEND_FACTOR_ZERO;
		blendStageInfo.alphaBlendOp						   = VK_BLEND_OP_ADD;
		blendStageInfo.colorWriteMask					   = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
															 VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo globalBlendStageInfo = {};
		globalBlendStageInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		globalBlendStageInfo.logicOpEnable						 = VK_FALSE;
		globalBlendStageInfo.logicOp							 = VK_LOGIC_OP_COPY;
		globalBlendStageInfo.attachmentCount					 = 1;
		globalBlendStageInfo.pAttachments						 = &blendStageInfo;
		globalBlendStageInfo.blendConstants[0]					 = 0.0f;
		globalBlendStageInfo.blendConstants[1]					 = 0.0f;
		globalBlendStageInfo.blendConstants[2]					 = 0.0f;
		globalBlendStageInfo.blendConstants[3]					 = 0.0f;

		VkPipelineDynamicStateCreateInfo dynamicStateStageInfo = {};
		dynamicStateStageInfo.sType							   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateStageInfo.dynamicStateCount				   = 0;
		dynamicStateStageInfo.pDynamicStates				   = nullptr;

		VkPipelineLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType					  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount			  = 0;
		layoutInfo.pSetLayouts				  = nullptr;
		layoutInfo.pushConstantRangeCount	  = 0;
		layoutInfo.pPushConstantRanges		  = nullptr;

		// Assert here as will do check on VkPipeline
		VULK_ASSERT_SUCCESS(vkCreatePipelineLayout, vulkanLogicalDevice.GetVkLogicalDevice(),  &layoutInfo, nullptr, &vulkPipelineLayout);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType						= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount					= nTypes_;
		pipelineCreateInfo.pStages						= shaderStageInfo;
		pipelineCreateInfo.pVertexInputState			= &vertexStageInfo;
		pipelineCreateInfo.pInputAssemblyState			= &assemblyStageInfo;
		pipelineCreateInfo.pViewportState				= &viewportStageInfo;
		pipelineCreateInfo.pRasterizationState			= &rasterStageInfo;
		pipelineCreateInfo.pMultisampleState			= &msaaStageInfo;
		pipelineCreateInfo.pDepthStencilState			= &depthStageInfo;
		pipelineCreateInfo.pColorBlendState				= &globalBlendStageInfo;
		pipelineCreateInfo.pDynamicState				= &dynamicStateStageInfo;
		pipelineCreateInfo.layout						= vulkPipelineLayout;
		pipelineCreateInfo.renderPass					= vulkRenderPass;
		pipelineCreateInfo.subpass						= 0;
		pipelineCreateInfo.basePipelineHandle			= VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex			= -1;

		return VULK_CHECK_SUCCESS(vkCreateGraphicsPipelines, vulkanLogicalDevice.GetVkLogicalDevice(), VK_NULL_HANDLE, 1,
															 &pipelineCreateInfo, nullptr, &vulkPipeline);
	}
}
