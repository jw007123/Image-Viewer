#include "Rendering/OpenGL/Backend.h"

namespace Rendering
{
	Backend::Backend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_) :
					 heapAllocator(heapAllocator_),
					 stackAllocator(stackAllocator_)
	{
		// Initialise glfw
		if (!glfwInit())
		{
			Utility::Log(Utility::Error, "Failed to init glfw!");
			assert(0);
		}

		// Define OpenGL version 4.5
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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
			Utility::Log(Utility::Error, "Failed to init gl3w!");
			assert(0);
		}

		// Define useful flags
		ImGuiIO& io		= ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// Init ImGui bindings
		ImGui_ImplOpenGL3_Init("#version 450");
		ImGui_ImplGlfw_InitForOpenGL(glWindow, true);
	}


	Backend::~Backend()
	{
		glfwDestroyWindow(glWindow);

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext(imguiContext);
	}


	bool Backend::IsRunning()
	{
		return !glfwWindowShouldClose(glWindow);
	}

	void Backend::StartFrame()
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


	void Backend::EndFrame()
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
