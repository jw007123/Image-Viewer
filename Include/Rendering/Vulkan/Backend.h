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

namespace Rendering
{
	class VulkBackend
	{
	public:
		VulkBackend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_);
		~VulkBackend();

		/// Is true for as long as the window is open
		bool IsRunning();

		void StartFrame();
		void EndFrame();

	private:
		VulkanGLFW			   vulkanGlfw;
		VulkanInstance		   vulkanInstance;
		VulkanPhysicalDevice   vulkanPhysicalDevice;
		VulkanQueueFamilies	   vulkanQueueFamilies;
		VulkanLogicalDevice	   vulkanLogicalDevice;
		VulkanVma			   vulkanVma;
		VulkanSurface		   vulkanSurface;
		VulkanSwapChain		   vulkanSwapChain;
		VulkanPipeline		   vulkanPipeline;
		VulkanImGui			   vulkanImGui;
	};
}
