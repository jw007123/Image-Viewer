#include "Rendering/Vulkan/Impl/VulkanVma.h"

namespace Rendering
{
	VulkanVma::VulkanVma(VulkanPhysicalDevice& vulkanPhysicalDevice_, VulkanLogicalDevice& vulkanLogicalDevice_,
						 VulkanInstance&	   vulkanInstance_) :
						 vulkanPhysicalDevice(vulkanPhysicalDevice_),
						 vulkanLogicalDevice(vulkanLogicalDevice_),
						 vulkanInstance(vulkanInstance_)
	{
		// For Vulkan .dll case
		VmaVulkanFunctions vmaVulkanFunctions    = {};
		vmaVulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vmaVulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo vmaAllocatorCreateInfo = {};
		vmaAllocatorCreateInfo.vulkanApiVersion		  = VK_API_VERSION_1_3;
		vmaAllocatorCreateInfo.physicalDevice		  = vulkanPhysicalDevice.GetVkPhysicalDevice();
		vmaAllocatorCreateInfo.device				  = vulkanLogicalDevice.GetVkLogicalDevice();
		vmaAllocatorCreateInfo.instance				  = vulkanInstance.GetVkInstance();
		vmaAllocatorCreateInfo.pVulkanFunctions		  = &vmaVulkanFunctions;

		VULK_ASSERT_SUCCESS(vmaCreateAllocator, &vmaAllocatorCreateInfo, &vmemAllocator);
	}


	VmaAllocator& VulkanVma::GetVmaAllocator()
	{
		return vmemAllocator;
	}


	VulkanVma::~VulkanVma()
	{
		vmaDestroyAllocator(vmemAllocator);
	}
}
