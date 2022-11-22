#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"

namespace Rendering
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
					   VulkanLogicalDevice&    vulkanLogicalDevice_);
		~VulkanPipeline();

		bool LoadShaderStage(const char* fName_, const VulkanShader::Type type_);
		bool LoadPipeline();

	private:
		struct Consts
		{
			static constexpr const char* shaderMainName								 = "main";
			static constexpr VkShaderStageFlagBits vkShaderStages[VulkanShader::Num] =
			{
				VK_SHADER_STAGE_VERTEX_BIT,
				VK_SHADER_STAGE_FRAGMENT_BIT
			};
		};
		VkPipeline vulkPipeline;

		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator&  heapAllocator;
		VulkanLogicalDevice&     vulkanLogicalDevice;

		VulkanShader vulkanShaders[VulkanShader::Num];

		/// Finishes the VkPipeline obj
		bool CreatePipeline(const u8 nTypes_, VulkanShader::Type* typesToUse_);
	};
}
