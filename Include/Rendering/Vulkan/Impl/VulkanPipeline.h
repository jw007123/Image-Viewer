#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"
#include "Utility/HeapAllocator.h"

#include "vma/vk_mem_alloc.h"

#include "Rendering/Vulkan/Impl/VulkanUtility.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"
#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanSwapChain.h"

namespace Rendering
{
	class VulkanPipeline
	{
	public:
		struct VertexInfo
		{
			VkVertexInputBindingDescription*   bindings;
			usize							   nBindings;
			VkVertexInputAttributeDescription* attributes;
			usize							   nAttributes;
		};

		struct UBOInfo
		{
			VkDescriptorSetLayout* layouts;
			usize				   nLayouts;
		};

		VulkanPipeline(Utility::HeapAllocator& heapAllocator_,       Utility::StackAllocator& stackAllocator_,
					   VulkanLogicalDevice&    vulkanLogicalDevice_, VulkanSwapChain&		  vulkanSwapChain_);
		~VulkanPipeline();

		void Destroy();

		bool LoadShaderStage(const char* fName_, const VulkanShader::Type type_);
		bool LoadPipeline(const VertexInfo& vertexInfo_, const UBOInfo& uboInfo_);

		bool GetVkPipeline(VkPipeline& vulkPipeline_);
		bool GetVkRenderPass(VkRenderPass& vulkRenderPass_);

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
		bool CreatePipeline(const VertexInfo& vertexInfo_, const UBOInfo& uboInfo_, const u8 nTypes_, VulkanShader::Type* typesToUse_);
	};
}
