#include "Rendering/Vulkan/Impl/VulkanImGui.h"

namespace Rendering
{
	VulkanImGui::VulkanImGui(VulkanGLFW& vulkanGLFW_)
	{
		// Setup ImGui
		imguiContext  = ImGui::CreateContext();
		imguiViewport = ImGui::GetMainViewport();
		ImGui::StyleColorsDark();

		// Define useful flags
		ImGuiIO& io		= ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Init ImGui bindings
		ImGui_ImplVulkan_InitInfo initInfo = {};
		ImGui_ImplVulkan_Init(&initInfo, VkRenderPass());
		ImGui_ImplGlfw_InitForVulkan(vulkanGLFW_.GetGLWindow(), true);
	}


	VulkanImGui::~VulkanImGui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(imguiContext);
	}
}
