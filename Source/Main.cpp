#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS

#include "Utility/Literals.h"
#include "Utility/TimeInterval.cpp"
#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "ImageProcessing/ImageTypes.h"
#include "ImageProcessing/Image.cpp"
#include "ImageProcessing/LuminanceFilter.cpp"
#include "ImageProcessing/WorkerThread.cpp"

#include "GUI/SizeConsts.h"
#include "GUI/MainMenuBar.cpp"
#include "GUI/FileMenu.cpp"
#include "GUI/Camera.cpp"
#include "GUI/Viewport.cpp"
#include "GUI/ZoomViewport.cpp"
#include "GUI/LuminanceOptions.cpp"
#include "GUI/OptionsPanel.cpp"

#include "Rendering/OpenGLBackend.cpp"
#include "Rendering/OpenGLShader.cpp"
#include "Rendering/OpenGLProgram.cpp"
#include "Rendering/OpenGLFramebuffer.cpp"
#include "Rendering/OpenGLTexture.cpp"
#include "Rendering/OpenGLRenderer.cpp"

void SendLuminanceRequest(const GUI::LuminanceOptions::Status& status_, std::vector<ImageProcessing::WorkerThread::Request>& addTo_)
{
	if (status_.flags == GUI::LuminanceOptions::Status::Flags::NoOp)
	{
		return;
	}

	ImageProcessing::WorkerThread::Request request;
	request.lumValue = status_;
	request.type	 = ImageProcessing::WorkerThread::Request::Type::Luminance;

	addTo_.push_back(request);
}


i16 main()
{
	Utility::HeapAllocator  heapAllocator;
	Utility::StackAllocator stackAllocator(1024 * 1024 * 10);

	ImageProcessing::Image image(heapAllocator);
	ImageProcessing::Image editingImage(heapAllocator);
	ImageProcessing::Image workerOutputImage(heapAllocator);

	// Setup worker thread
	ImageProcessing::WorkerThread::InitialData threadData(image, workerOutputImage);
	std::thread								   workerThread(ImageProcessing::WorkerThread::Main, &threadData);
	while (!threadData.threadReady.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Rendering::OpenGLBackend  glBackend;
	Rendering::OpenGLRenderer glRenderer(stackAllocator, heapAllocator);

	GUI::MainMenuBar  mainMenuBar(image);
	GUI::Viewport	  viewport(heapAllocator, stackAllocator, glRenderer);
	GUI::ZoomViewport zoomViewport(heapAllocator, stackAllocator, glRenderer);
	GUI::OptionsPanel optionsPanel(heapAllocator, stackAllocator);

	Utility::TimeInterval renderInterval(1.0f / 165.0f);
	while (glBackend.IsRunning())
	{
		glBackend.StartFrame();
		{
			// Check thread output
			if (threadData.outputReady.load())
			{
				threadData.outputMutex.lock();
				{
					editingImage.Copy(threadData.outputImage);
					glRenderer.UpdateTexture(editingImage);

					threadData.outputReady.store(false);
				}
				threadData.outputMutex.unlock();
			}

			// Check user image input
			GUI::MainMenuBar::Status mainMenuBarStatus;
			threadData.inputMutex.lock();
			{
				mainMenuBarStatus = mainMenuBar.Draw();
			}
			threadData.inputMutex.unlock();

			// Setup image copies
			if (mainMenuBarStatus.fileStatus.flags == GUI::FileMenu::Status::Open)
			{
				editingImage.Copy(image);
				glRenderer.UpdateTexture(editingImage);
			}

			// Draw viewports
			const GUI::Viewport::Status viewportStatus		   = viewport.Draw();
			const GUI::ZoomViewport::Status zoomViewportStatus = zoomViewport.Draw(viewportStatus.cameraPos);

			// Send any requests to thread
			if (image.IsValid())
			{
				const GUI::OptionsPanel::Status panelStatus = optionsPanel.Draw();
				threadData.requestsMutex.lock();
				{
					SendLuminanceRequest(panelStatus.lumStatus, threadData.requests);
				}
				threadData.requestsMutex.unlock();
			}
		}
		glBackend.EndFrame();

		renderInterval.Wait();
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
