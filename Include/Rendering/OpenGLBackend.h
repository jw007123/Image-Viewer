#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include "GUI/SizeConsts.h"

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Rendering
{
	class OpenGLBackend
	{
	public:
		OpenGLBackend();
		~OpenGLBackend();

		/// Is true for as long as the window is open
		bool IsRunning();

		void StartFrame();
		void EndFrame();

	private:

		GLFWwindow* glWindow;

		ImGuiContext*  imguiContext;
		ImGuiViewport* imguiViewport;
	};
}
