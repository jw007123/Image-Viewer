#include "Rendering/OpenGLBackend.h"

namespace Rendering
{
	OpenGLBackend::OpenGLBackend()
	{
		// Initialise glfw
		if (!glfwInit())
		{
			Utility::Log(Utility::LogFlag::Error, "Failed to init glfw!");
			assert(0);
		}

		// Define OpenGL version 4.5
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_SAMPLES, 8);

		// Setup window
		const usize width  = GUI::SizeConsts::viewportWidth	+ GUI::SizeConsts::optionsPanelWidth;
		const usize height = std::max<usize>(GUI::SizeConsts::optionsPanelWidth, GUI::SizeConsts::optionsPanelHeight);

		glWindow = glfwCreateWindow(width, height, "ImageViewer", NULL, NULL);
		if (!glWindow)
		{
			Utility::Log(Utility::LogFlag::Error, "Failed to create a glfw::Window(%zu, %zu) object!", width, height);
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
			Utility::Log(Utility::LogFlag::Error, "Failed to init gl3w!");
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


	bool OpenGLBackend::IsRunning()
	{
		return !glfwWindowShouldClose(glWindow);
	}

	void OpenGLBackend::StartFrame()
	{
		// Read user IO
		glfwPollEvents();

		// Clear frame
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


	void OpenGLBackend::EndFrame()
	{
		// Set VP size
		i16 width  = 0;
		i16 height = 0;
		glfwGetFramebufferSize(glWindow, &width, &height);
		glViewport(0, 0, width, height);

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Display back buffer to user
		glfwSwapBuffers(glWindow);
	}
}
