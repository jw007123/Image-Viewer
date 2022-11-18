#include "Rendering/Vulkan/Impl/VulkanImGui.h"

namespace Rendering
{
	VulkanImGui::VulkanImGui(GLFWwindow* glWindow_)
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
		//ImGui_ImplVulkan_Init();
		ImGui_ImplGlfw_InitForVulkan(glWindow_, true);
	}


	VulkanImGui::~VulkanImGui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(imguiContext);
	}
}
