#include "Rendering/Vulkan/Backend.h"

namespace Rendering
{
	VulkBackend::VulkBackend(Utility::StackAllocator& stackAllocator_) :
						     vulkGlfw(),
							 vulkInstance(stackAllocator_),
							 vulkImGui(vulkGlfw.GetWindow())
	{
		
	}


	VulkBackend::~VulkBackend()
	{
		
	}


	bool VulkBackend::IsRunning()
	{
		return vulkGlfw.IsWindowOpen();
	}
}
