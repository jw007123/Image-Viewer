#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include "GUI/SizeConsts.h"

#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace Rendering
{
	class VulkanImGui
	{
	public:
		VulkanImGui(GLFWwindow* glWindow_);
		~VulkanImGui();

	private:
		ImGuiContext*  imguiContext;
		ImGuiViewport* imguiViewport;
	};
}
