#include "Rendering/Vulkan/Impl/VulkanGLFW.h"

namespace Rendering
{
	VulkanGLFW::VulkanGLFW()
	{
		// Initialise glfw
		if (!glfwInit())
		{
			Utility::Log(Utility::Error, "Failed to init glfw!");
			assert(0);
		}

		// Tell glfw to use Vulkan
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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
	}


	VulkanGLFW::~VulkanGLFW()
	{
		glfwDestroyWindow(glWindow);
		glfwTerminate();
	}


	bool VulkanGLFW::IsWindowOpen()
	{
		return !glfwWindowShouldClose(glWindow);
	}


	void VulkanGLFW::PollEvents()
	{
		glfwPollEvents();
	}


	GLFWwindow* const VulkanGLFW::GetGLWindow() const
	{
		return glWindow;
	}
}
