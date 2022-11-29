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
#include "Rendering/Vulkan/Impl/VulkanMeshData.h"
#include "Rendering/Vulkan/Impl/VulkanUBO.h"

namespace Rendering
{
	class VRenderer;

	class VBackend
	{
	public:
		VBackend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_);
		~VBackend();

		bool IsRunning();

		void StartFrame();
		void RenderFullView(const GUI::Camera& cam_, const f32 aspectRatio_);
		void EndFrame();

	private:
		struct Consts
		{
			static constexpr usize maxFramesInFlight = 2;
		};

		friend class VRenderer;

		uint32_t    imageTargetIdx;
		uint32_t	currentFrameIdx;
		VkSemaphore vulkImageAvailableSem[Consts::maxFramesInFlight];
		VkSemaphore vulkImageFinishedSem[Consts::maxFramesInFlight];
		VkFence		vulkInFlightFence[Consts::maxFramesInFlight];

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;
		bool					 isMinimised;

		VulkanGLFW			   vulkanGlfw;
		VulkanInstance		   vulkanInstance;
		VulkanPhysicalDevice   vulkanPhysicalDevice;
		VulkanQueueFamilies	   vulkanQueueFamilies;
		VulkanLogicalDevice	   vulkanLogicalDevice;
		VulkanVma			   vulkanVma;
		VulkanSurface		   vulkanSurface;
		VulkanSwapChain		   vulkanSwapChain;
		VulkanCommandPool	   vulkanCommandPool[Consts::maxFramesInFlight];

		VulkanUBO         cameraDataUBO[Consts::maxFramesInFlight];
		VulkanMeshData	  quadMeshData;
		VulkanPipeline	  mainViewportPipeline;
		VulkanFramebuffer vulkanFramebuffer;
		VulkanImGui		  vulkanImGui;

		void LoadTextureMesh();
		void LoadPipeline();

		void RecordFullViewToBuffer();
	};
}
