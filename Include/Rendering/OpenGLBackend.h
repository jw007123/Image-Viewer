#include "Utility/Literals.h"

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

	private:
		GLFWwindow* glWindow;

		ImGuiContext*  imguiContext;
		ImGuiViewport* imguiViewport;
	};
}
