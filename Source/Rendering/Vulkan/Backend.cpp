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
							 vulkanImGui(vulkanGlfw)
	{

		VulkanShader vsShader(vulkanLogicalDevice);
		vsShader.Load(stackAllocator_, VulkanShader::Vertex, "vsTest");

		VulkanShader fsShader(vulkanLogicalDevice);
		fsShader.Load(stackAllocator_, VulkanShader::Fragment, "fsTest");
	}


	VulkBackend::~VulkBackend()
	{

	}


	bool VulkBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}
}
