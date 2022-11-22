#include "Rendering/Vulkan/Impl/VulkanPipeline.h"

namespace Rendering
{
	VulkanPipeline::VulkanPipeline(Utility::HeapAllocator& heapAllocator_,	     Utility::StackAllocator& stackAllocator_,
								   VulkanLogicalDevice&    vulkanLogicalDevice_) :
								   heapAllocator(heapAllocator_),
								   stackAllocator(stackAllocator_),
								   vulkanLogicalDevice(vulkanLogicalDevice_),
								   vulkanShaders{ vulkanLogicalDevice, vulkanLogicalDevice }
	{

	}


	VulkanPipeline::~VulkanPipeline()
	{

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

		return CreatePipeline(nTypes, typesToUse);
	}


	bool VulkanPipeline::CreatePipeline(const u8 nTypes_, VulkanShader::Type* typesToUse_)
	{
		// Create structs for each shader we want to make use of
		VkPipelineShaderStageCreateInfo createInfos[VulkanShader::Num];
		for (u8 i = 0; i < nTypes_; ++i)
		{
			VkPipelineShaderStageCreateInfo& createInfo = createInfos[i];
			createInfo								    = {};
			createInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.stage							= Consts::vkShaderStages[typesToUse_[i]];
			createInfo.module							= vulkanShaders[typesToUse_[i]].GetVkShaderModule();
			createInfo.pName							= Consts::shaderMainName;
			createInfo.pSpecializationInfo				= nullptr;
		}

		return true;
	}
}
