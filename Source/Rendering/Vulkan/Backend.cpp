#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VulkBackend::VulkBackend(Utility::StackAllocator& stackAllocator)
	{
		// Initialise glfw
		if (!glfwInit())
		{
			Utility::Log(Utility::Error, "Failed to init glfw!");
			assert(0);
		}

		// Tell glfw to use Vulkan
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, 8);

		// Setup window
		const usize width  = (1.0f / GUI::SizeConsts::viewportOptionsRatioX) * GUI::SizeConsts::viewportWidth;
		const usize height = GUI::SizeConsts::viewportHeight;

		glWindow = glfwCreateWindow(width, height, "ImageViewer", NULL, NULL);
		if (!glWindow)
		{
			Utility::Log(Utility::Error, "Failed to create a glfw::Window(%zu, %zu) object!", width, height);
			assert(0);
		}

		// Setup ImGui
		imguiContext  = ImGui::CreateContext();
		imguiViewport = ImGui::GetMainViewport();
		ImGui::StyleColorsDark();

		// Define useful flags
		ImGuiIO& io     = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Init Vulkan objs
		vulkInstance.Init(stackAllocator);

		// Init ImGui bindings
		ImGui_ImplVulkan_InitInfo initInfo = {};
		//ImGui_ImplVulkan_Init();
		ImGui_ImplGlfw_InitForVulkan(glWindow, true);
	}


	VulkBackend::~VulkBackend()
	{
		glfwDestroyWindow(glWindow);
		glfwTerminate();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(imguiContext);
	}


	bool VulkBackend::IsRunning()
	{
		return !glfwWindowShouldClose(glWindow);
	}
}
