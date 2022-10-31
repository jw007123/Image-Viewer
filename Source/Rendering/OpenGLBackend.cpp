#include "Rendering/OpenGLBackend.h"

namespace Rendering
{
	OpenGLBackend::OpenGLBackend()
	{
		// Initialise glfw
		if (!glfwInit())
		{
			assert(0);
		}

		// Define OpenGL version 4.5
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Setup window
		glWindow = glfwCreateWindow(1280, 720, "ImageViewer", NULL, NULL);
		if (!glWindow)
		{
			assert(0);
		}

		// Define glWindow as render target
		glfwSwapInterval(0);
		glfwMakeContextCurrent(glWindow);

		// Setup ImGui
		imguiContext  = ImGui::CreateContext();
		imguiViewport = ImGui::GetMainViewport();
		ImGui::StyleColorsDark();

		// Init gl3w. Non-zero implies error
		if (gl3wInit())
		{
			assert(0);
		}

		// Define useful flags
		ImGuiIO& io		= ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Init ImGui bindings
		ImGui_ImplOpenGL3_Init("#version 450");
		ImGui_ImplGlfw_InitForOpenGL(glWindow, true);
	}


	OpenGLBackend::~OpenGLBackend()
	{
		glfwDestroyWindow(glWindow);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(imguiContext);
	}
}
