#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS
#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define VMA_IMPLEMENTATION

#include "Utility/Literals.h"
#include "Utility/TimeInterval.cpp"
#include "Utility/MemoryBlock.cpp"
#include "Utility/Log.cpp"
#include "Utility/StackAllocator.cpp"
#include "Utility/HeapAllocator.cpp"

#include "ImageProcessing/ImageTypes.h"
#include "ImageProcessing/Image.cpp"
#include "ImageProcessing/WorkerThread.cpp"
#include "ImageProcessing/Filters/Luminance.cpp"
#include "ImageProcessing/Filters/Saturation.cpp"

#include "GUI/SizeConsts.h"
#include "GUI/MainMenu.cpp"
#include "GUI/Camera.cpp"
#include "GUI/Viewport.cpp"
#include "GUI/ZoomViewport.cpp"
#include "GUI/OptionsPanel.cpp"
#include "GUI/Options/Luminance.cpp"
#include "GUI/Options/Saturation.cpp"

#include "Rendering/API.cpp"

//// TEMP ////
#include "Rendering/Vulkan/Impl/VulkanImGui.cpp"
#include "Rendering/Vulkan/Impl/VulkanGLFW.cpp"
#include "Rendering/Vulkan/Impl/VulkanUtility.cpp"
#include "Rendering/Vulkan/Impl/VulkanInstance.cpp"
#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.cpp"
#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.cpp"
#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.cpp"
#include "Rendering/Vulkan/Impl/VulkanVma.cpp"
#include "Rendering/Vulkan/Impl/VulkanSurface.cpp"
#include "Rendering/Vulkan/Impl/VulkanSwapChain.cpp"
#include "Rendering/Vulkan/Impl/VulkanShader.cpp"
#include "Rendering/Vulkan/Impl/VulkanPipeline.cpp"
#include "Rendering/Vulkan/Impl/VulkanFramebuffer.cpp"
#include "Rendering/Vulkan/Impl/VulkanCommandPool.cpp"
#include "Rendering/Vulkan/Impl/VulkanMeshData.cpp"
#include "Rendering/Vulkan/Impl/VulkanUBO.cpp"
#include "Rendering/Vulkan/Impl/VulkanPushConstant.cpp"
#include "Rendering/Vulkan/Backend.cpp"

i16 main()
{
	Utility::HeapAllocator  heapAllocator;
	Utility::StackAllocator stackAllocator(1024 * 1024 * 10);

	ImageProcessing::Image image(heapAllocator);
	ImageProcessing::Image editingImage(heapAllocator);
	ImageProcessing::Image workerOutputImage(heapAllocator);

	Rendering::VBackend  backend(heapAllocator, stackAllocator);

	// Setup worker thread
	ImageProcessing::WorkerThread::InitialData threadData(image, workerOutputImage);
	std::thread								   workerThread(ImageProcessing::WorkerThread::Main, &threadData);
	while (!threadData.threadReady.load())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	GUI::Camera cam(16.0f / 9.0f, -1.0f);

	// Render loop
	Utility::TimeInterval renderInterval(1.0f / 165.0f);
	while (backend.IsRunning())
	{
		backend.StartFrame();
		{
			backend.RenderFullView(cam, 16.0f / 9.0f);
		}
		backend.EndFrame();

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
