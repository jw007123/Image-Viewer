#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"

namespace Rendering
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(Utility::StackAllocator& stackAllocator_, VulkanInstance& vulkanInstance_) :
											   vulkanInstance(vulkanInstance_)
	{
		// Determine the number of physical devices
		uint32_t nPhysicalDevices = 0;
		VULK_ASSERT_SUCCESS(vkEnumeratePhysicalDevices, vulkanInstance.GetVkInstance(), &nPhysicalDevices, nullptr);
		if (!nPhysicalDevices)
		{
			assert(0);
		}

		// Allocate memory and obtain list
		Utility::MemoryBlock pyhsicalDeviceBlk = stackAllocator_.Allocate<VkPhysicalDevice>(nPhysicalDevices);
		VkPhysicalDevice* physicalDevices	   = (VkPhysicalDevice*)pyhsicalDeviceBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumeratePhysicalDevices, vulkanInstance.GetVkInstance(), &nPhysicalDevices, physicalDevices);

		// Determine physical device to use
		for (usize i = 0; i < nPhysicalDevices; ++i)
		{
			if (CheckRequiredDeviceProperties(stackAllocator_, physicalDevices[i]))
			{
				vulkPhysicalDevice = physicalDevices[i];
				break;
			}
		}

		// Free mem
		stackAllocator_.Free(pyhsicalDeviceBlk);
	}


	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
		// Physical device is destroyed as part of instance, so no need for any calls here
	}


	VkPhysicalDevice& VulkanPhysicalDevice::GetVkPhysicalDevice()
	{
		return vulkPhysicalDevice;
	}


	bool VulkanPhysicalDevice::CheckRequiredDeviceProperties(Utility::StackAllocator& stackAllocator_, const VkPhysicalDevice& testVulkDevice_) const
	{
		VkPhysicalDeviceProperties vulkProperties;
		VkPhysicalDeviceFeatures   vulkFeatures;

		// Get properties and features to check against
		vkGetPhysicalDeviceProperties(testVulkDevice_, &vulkProperties);
		vkGetPhysicalDeviceFeatures(testVulkDevice_,   &vulkFeatures);

		return (vulkProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
	}
}
