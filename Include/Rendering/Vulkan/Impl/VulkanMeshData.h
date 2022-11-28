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
	struct VulkanMeshData
	{
		VulkanMeshData(Utility::StackAllocator& stackAllocator_, VmaAllocator& vulkAllocator_);
		~VulkanMeshData();

		void Create(Eigen::Vector3f* points_,	 const usize nPoints_,
					Eigen::Vector2f* texCoords_, const usize nTexCoords_,
					uint32_t*		 indices_,	 const usize nIndices_);

		VmaAllocator&			 vulkAllocator;
		Utility::StackAllocator& stackAllocator;

		VmaAllocation	  vertAllocation;
		VmaAllocationInfo vertAllocationInfo;
		VkBuffer		  vertBuffer;
		VmaAllocation	  indicesAllocation;
		VmaAllocationInfo indicesAllocationInfo;
		VkBuffer		  indicesBuffer;

		VkVertexInputBindingDescription   vulkBindingDescription;
		VkVertexInputAttributeDescription vulkAttributeDescription[2];
	};
}
