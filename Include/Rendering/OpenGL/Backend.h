#pragma once

#include "Utility/Literals.h"
#include "Utility/Log.h"
#include "Utility/HeapAllocator.h"
#include "Utility/StackAllocator.h"

#include "GUI/SizeConsts.h"

#include <GL/gl3w.h>       
#include <GLFW/glfw3.h>

#include "imgui_internal.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace Rendering
{
	class Renderer;

	class Backend
	{
	public:
		Backend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_);
		~Backend();

		/// Is true for as long as the window is open
		bool IsRunning();

		void StartFrame();
		void EndFrame();

	private:
		friend class Renderer;

		Utility::HeapAllocator&  heapAllocator;
		Utility::StackAllocator& stackAllocator;

		GLFWwindow* glWindow;

		ImGuiContext*  imguiContext;
		ImGuiViewport* imguiViewport;
	};
}
