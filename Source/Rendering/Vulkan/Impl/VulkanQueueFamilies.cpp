#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"

namespace Rendering
{
	VulkanQueueFamilies::VulkanQueueFamilies(Utility::StackAllocator& stackAllocator_, VulkanPhysicalDevice& vulkanPhysicalDevice_) :
											 vulkanPhysicalDevice(vulkanPhysicalDevice_)
	{
		// Unset all queues
		for (u8 i = 0; i < IDs::Num; ++i)
		{
			queueHandles[i] = -1;
		}

		// Determine number of queues
		uint32_t nQueueFamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vulkanPhysicalDevice.GetVkPhysicalDevice(), &nQueueFamilies, nullptr);
		if (!nQueueFamilies)
		{
			assert(0);
		}

		// Allocate memory and get queue info
		Utility::MemoryBlock queueFamiliesBlk  = stackAllocator_.Allocate<VkQueueFamilyProperties>(nQueueFamilies);
		VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)queueFamiliesBlk.ptr;
		vkGetPhysicalDeviceQueueFamilyProperties(vulkanPhysicalDevice.GetVkPhysicalDevice(), &nQueueFamilies, queueFamilies);

		// Set queue indices
		for (u8 i = 0; i < IDs::Num; ++i)
		{
			for (usize j = 0; j < nQueueFamilies; ++j)
			{
				const bool queueHasSupport = (Consts::requiredQueueBits[i] & queueFamilies[j].queueFlags);
				if (queueHasSupport)
				{
					queueHandles[i] = j;
					break;
				}
			}

			if (queueHandles[i] == -1)
			{
				// Failed to find queue
				assert(0);
			}
		}

		// Free mem
		stackAllocator_.Free(queueFamiliesBlk);
	}


	VulkanQueueFamilies::~VulkanQueueFamilies()
	{
		// Queues destroyed at same time as physical device
	}


	uint32_t VulkanQueueFamilies::GetQueueHandle(const IDs idx_) const
	{
		return queueHandles[idx_];
	}


	bool VulkanQueueFamilies::IsValid() const
	{
		for (u8 i = 0; i < IDs::Num; ++i)
		{
			if (queueHandles[i] == -1)
			{
				return false;
			}
		}

		return true;
	}
}
