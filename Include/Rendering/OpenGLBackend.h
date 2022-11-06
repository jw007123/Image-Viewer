#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
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
		struct Consts
		{
			static constexpr usize startWidth  = 1280;
			static constexpr usize startHeight = 720;
		};

		GLFWwindow* glWindow;

		ImGuiContext*  imguiContext;
		ImGuiViewport* imguiViewport;
	};
}
