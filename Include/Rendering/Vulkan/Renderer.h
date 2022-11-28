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

#include "Rendering/Vulkan/Impl/VulkanImGui.h"
#include "Rendering/Vulkan/Impl/VulkanGLFW.h"
#include "Rendering/Vulkan/Impl/VulkanInstance.h"
#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"
#include "Rendering/Vulkan/Impl/VulkanVma.h"
#include "Rendering/Vulkan/Impl/VulkanSurface.h"
#include "Rendering/Vulkan/Impl/VulkanSwapChain.h"
#include "Rendering/Vulkan/Impl/VulkanShader.h"
#include "Rendering/Vulkan/Impl/VulkanPipeline.h"
#include "Rendering/Vulkan/Impl/VulkanFramebuffer.h"
#include "Rendering/Vulkan/Impl/VulkanCommandPool.h"

#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	class VRenderer
	{
	public:
		VRenderer(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_, VBackend& backend_);
		~VRenderer();

		void RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_);

	private:
		struct MeshVulkanData
		{
			MeshVulkanData(Utility::HeapAllocator& heapAllocator_, VmaAllocator& vulkAllocator_);
			~MeshVulkanData();

			void Create(Eigen::Vector3f* points_,    const usize nPoints_,
						Eigen::Vector2f* texCoords_, const usize nTexCoords_);

			VmaAllocator&			vulkAllocator;
			VmaAllocation			vertAllocation;
			VkBuffer			    vertBuffer;
			Utility::HeapAllocator& heapAllocator;
			Utility::MemoryBlock	meshDataBlk;

			VkVertexInputBindingDescription   vulkBindingDescription;
			VkVertexInputAttributeDescription vulkAttributeDescription[2];
		};

		VBackend&			     backend;
		Utility::StackAllocator& stackAllocator;
		Utility::HeapAllocator&  heapAllocator;

		MeshVulkanData			 quadMeshData;

		VulkanPipeline		   mainViewportPipeline;
		VulkanFramebuffer	   vulkanFramebuffer;

		void LoadTextureMesh();
	};
}
