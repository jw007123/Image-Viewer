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

namespace Rendering
{
	class VulkBackend
	{
	public:
		VulkBackend(Utility::StackAllocator& stackAllocator_);
		~VulkBackend();

		/// Is true for as long as the window is open
		bool IsRunning();

		void StartFrame();
		void EndFrame();

	private:
		VulkanImGui	   vulkImGui;
		VulkanGLFW	   vulkGlfw;
		VulkanInstance vulkInstance;
	};
}
