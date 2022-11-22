#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VulkBackend::VulkBackend(Utility::HeapAllocator& heapAllocator_, Utility::StackAllocator& stackAllocator_) :
						     vulkanGlfw(),
							 vulkanInstance(stackAllocator_),
							 vulkanPhysicalDevice(stackAllocator_, vulkanInstance),
							 vulkanQueueFamilies(stackAllocator_, vulkanPhysicalDevice),
							 vulkanLogicalDevice(stackAllocator_, vulkanQueueFamilies, vulkanPhysicalDevice),
							 vulkanVma(vulkanPhysicalDevice, vulkanLogicalDevice, vulkanInstance),
							 vulkanSurface(vulkanInstance, vulkanGlfw, vulkanQueueFamilies, vulkanPhysicalDevice, heapAllocator_),
							 vulkanSwapChain(heapAllocator_, vulkanPhysicalDevice, vulkanSurface, vulkanQueueFamilies, vulkanLogicalDevice, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT),
							 vulkanPipeline(heapAllocator_, stackAllocator_, vulkanLogicalDevice, vulkanSwapChain),
							 vulkanImGui(vulkanGlfw)
	{

		vulkanPipeline.LoadShaderStage("vsTest", VulkanShader::Vertex);
		vulkanPipeline.LoadShaderStage("fsTest", VulkanShader::Fragment);
		vulkanPipeline.LoadPipeline();
	}


	VulkBackend::~VulkBackend()
	{

	}


	bool VulkBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}
}
