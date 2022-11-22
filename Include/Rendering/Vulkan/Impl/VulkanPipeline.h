#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"

namespace Rendering
{
	class VulkanPipeline
	{
	public:
		VulkanPipeline(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
					   VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanSwapChain&		  vulkanSwapChain_);
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
				VK_SHADER_STAGE_FRAGMENT_BIT,
				VK_SHADER_STAGE_GEOMETRY_BIT
			};
		};

		VulkanLogicalDevice& vulkanLogicalDevice;
		VulkanSwapChain&     vulkanSwapChain;

		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator&  heapAllocator;

		VkPipeline		 vulkPipeline;
		VkPipelineLayout vulkPipelineLayout;
		VkRenderPass	 vulkRenderPass;
		bool			 wasPipelineCreated;
		bool			 wasRenderPassCreated;
		VulkanShader	 vulkanShaders[VulkanShader::Num];

		/// Completes the VkRenderPass member
		bool CreateRenderPass();

		/// Completes the VkPipelineLayout and VkPipeline members
		bool CreatePipeline(const u8 nTypes_, VulkanShader::Type* typesToUse_);
	};
}
