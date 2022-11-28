#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/StackAllocator.h"

#include "GUI/SizeConsts.h"

#include <GLFW/glfw3.h>

#include "vma/vk_mem_alloc.h"

#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "Rendering/Vulkan/Impl/VulkanVma.h"

namespace Rendering
{
	struct VulkanUBO
	{
		struct CreateInfo
		{
			uint32_t		   bindingNo;
			VkShaderStageFlags shaderStageFlags;
		};

		VulkanUBO(Utility::StackAllocator& stackAllocator_, VmaAllocator& vulkAllocator_, VulkanLogicalDevice& vulkLogicalDevice_);
		~VulkanUBO();

		void Create(const CreateInfo& createInfo_, const Utility::MemoryBlock dataBlk_);
		void Update(const Utility::MemoryBlock dataBlk_);
	
		VmaAllocator&			 vulkAllocator;
		Utility::StackAllocator& stackAllocator;
		VulkanLogicalDevice&     vulkLogicalDevice;

		bool				  created;
		VkDescriptorSetLayout uboLayout;
	};
}
