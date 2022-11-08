#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS

#include "Utility/Literals.h"
#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "ImageProcessing/ImageTypes.h"
#include "ImageProcessing/Image.cpp"
#include "ImageProcessing/WorkerThread.cpp"

#include "GUI/SizeConsts.h"
#include "GUI/MainMenuBar.cpp"
#include "GUI/FileMenu.cpp"
#include "GUI/Camera.cpp"
#include "GUI/Viewport.cpp"
#include "GUI/LuminanceOptions.cpp"
#include "GUI/OptionsPanel.cpp"

#include "Rendering/OpenGLBackend.cpp"
#include "Rendering/OpenGLShader.cpp"
#include "Rendering/OpenGLProgram.cpp"
#include "Rendering/OpenGLFramebuffer.cpp"
#include "Rendering/OpenGLTexture.cpp"
#include "Rendering/OpenGLRenderer.cpp"

i16 main()
{
	Utility::HeapAllocator  heapAllocator;
	Utility::StackAllocator stackAllocator(1024 * 1024 * 10);
	ImageProcessing::Image  image(heapAllocator);

	// Setup worker thread
	ImageProcessing::WorkerThread::InitialData threadData(image);
	std::thread								   workerThread(ImageProcessing::WorkerThread::Main, &threadData);
	while (!threadData.threadReady.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Setup rendering
	Rendering::OpenGLBackend glBackend;
	GUI::MainMenuBar		 mainMenuBar(threadData.inputMutex, image);
	GUI::Viewport			 viewport(heapAllocator, stackAllocator, image);
	GUI::OptionsPanel		 optionsPanel(heapAllocator, stackAllocator, image);

	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			const GUI::MainMenuBar::Status mainMenuBarStatus = mainMenuBar.Draw();
			const GUI::Viewport::Status viewportStatus		 = viewport.Draw();
			const GUI::OptionsPanel::Status panelStatus		 = optionsPanel.Draw();
		}
		glBackend.EndFrame();
	}

	// Shutdown thread
	threadData.threadShutdown.store(true);
	while (threadData.threadReady.load())
	{
		// Wait for thread to signal shutdown
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	workerThread.join();

	return -1;
}
