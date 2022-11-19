#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VulkBackend::VulkBackend(Utility::StackAllocator& stackAllocator_) :
						     vulkanGlfw(),
							 vulkanInstance(stackAllocator_),
							 vulkanPhysicalDevice(stackAllocator_, vulkanInstance),
							 vulkanQueueFamilies(stackAllocator_, vulkanPhysicalDevice),
							 vulkanLogicalDevice(stackAllocator_, vulkanQueueFamilies, vulkanPhysicalDevice),
							 vulkanImGui(vulkanGlfw)
	{
		
	}


	VulkBackend::~VulkBackend()
	{
		
	}


	bool VulkBackend::IsRunning()
	{
		return vulkanGlfw.IsWindowOpen();
	}
}
