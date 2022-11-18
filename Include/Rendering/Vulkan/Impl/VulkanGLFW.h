#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include <GLFW/glfw3.h>

namespace Rendering
{
	class VulkanGLFW
	{
	public:
		VulkanGLFW();
		~VulkanGLFW();

		bool IsWindowOpen();

		GLFWwindow* const GetWindow() const;

	private:
		GLFWwindow* glWindow;
	};
}
