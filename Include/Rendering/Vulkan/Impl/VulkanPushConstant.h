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
	struct VulkanPushConstant
	{
		static constexpr uint32_t maxSizeBound = 1024;

		struct CreateInfo
		{
			VkShaderStageFlags shaderStageFlags;
			usize			   dataSize;
		};

		VulkanPushConstant(Utility::HeapAllocator& heapAllocator_, VulkanPhysicalDevice& vulkPhysicalDevice_);
		~VulkanPushConstant();

		void Create(const CreateInfo& createInfo_);
		void Update(const Utility::MemoryBlock dataBlk_);

		Utility::HeapAllocator& heapAllocator;
		VulkanPhysicalDevice&   vulkPhysicalDevice;

		VkPushConstantRange  pushConstant;
		Utility::MemoryBlock dataBlk;
	};
}
