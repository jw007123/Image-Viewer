#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"

namespace Rendering
{
	VulkanLogicalDevice::VulkanLogicalDevice(Utility::StackAllocator& stackAllocator_, VulkanQueueFamilies& vulkanQueueFamilies_,
																					   VulkanPhysicalDevice& vulkanPhysicalDevice_) :
											 vulkanQueueFamilies(vulkanQueueFamilies_),
											 vulkanPhysicalDevice(vulkanPhysicalDevice_)
	{
		// Check that we have all the extensions we want
		if (!CheckRequiredExtensions(stackAllocator_, Consts::nRequiredExtensions, Consts::requiredExtensions))
		{
			assert(0);
		}

		// Setup QueueCreateInfo struct for each unique queue handle
		VkDeviceQueueCreateInfo vulkQueueCreateInfos[VulkanQueueFamilies::Num];
		usize nUniqueHandles = 0;
		for (u8 i = 0; i < VulkanQueueFamilies::Num; ++i)
		{
			const uint32_t curHandle = vulkanQueueFamilies.GetQueueHandle((VulkanQueueFamilies::IDs)i);
			bool isUnique			 = true;
			for (u8 j = 0; j < i; ++j)
			{
				if (curHandle == vulkanQueueFamilies.GetQueueHandle((VulkanQueueFamilies::IDs)j))
				{
					isUnique = false;
					break;
				}
			}

			// Only fill in struct if we're on a unique queue handle
			nUniqueHandles += isUnique;
			if (isUnique)
			{
				const f32 queuePriority					 = 1.0f;
				vulkQueueCreateInfos[i]					 = {};
				vulkQueueCreateInfos[i].sType			 = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				vulkQueueCreateInfos[i].queueFamilyIndex = vulkanQueueFamilies.GetQueueHandle((VulkanQueueFamilies::IDs)i);
				vulkQueueCreateInfos[i].queueCount		 = 1;
				vulkQueueCreateInfos[i].pQueuePriorities = &queuePriority;
			}
		}
		
		VkPhysicalDeviceFeatures vulkDeviceFeatures = {};
		// Left blank at the moment

		VkDeviceCreateInfo vulkDeviceCreateInfo      = {};
		vulkDeviceCreateInfo.sType			         = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vulkDeviceCreateInfo.pQueueCreateInfos	     = vulkQueueCreateInfos;
		vulkDeviceCreateInfo.queueCreateInfoCount    = nUniqueHandles;
		vulkDeviceCreateInfo.pEnabledFeatures        = &vulkDeviceFeatures;
		vulkDeviceCreateInfo.enabledExtensionCount   = Consts::nRequiredExtensions;
		vulkDeviceCreateInfo.ppEnabledExtensionNames = Consts::requiredExtensions;

#ifdef NDEBUG
		// Disable validation layers in release
		vulkDeviceCreateInfo.enabledLayerCount   = 0;
		vulkDeviceCreateInfo.ppEnabledLayerNames = nullptr;
#else
		// Check required layers are available
		if (!CheckRequiredValidationLayers(stackAllocator_, Consts::nRequiredLayers, Consts::debugValidationLayer))
		{
			assert(0);
		}

		vulkDeviceCreateInfo.enabledLayerCount   = Consts::nRequiredLayers;
		vulkDeviceCreateInfo.ppEnabledLayerNames = Consts::debugValidationLayer;
#endif

		// At long last, create the logical device
		VULK_ASSERT_SUCCESS(vkCreateDevice, vulkanPhysicalDevice.GetVkPhysicalDevice(), &vulkDeviceCreateInfo, nullptr, &vulkLogicalDevice);

		// Setup queues now that we have a valid device
		for (u8 i = 0; i < VulkanQueueFamilies::Num; ++i)
		{
			const VulkanQueueFamilies::IDs iAsIdx = (VulkanQueueFamilies::IDs)i;
			vkGetDeviceQueue(vulkLogicalDevice, vulkanQueueFamilies.GetQueueHandle(iAsIdx), 0, &vulkQueues[i]);
		}
	}


	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		vkDestroyDevice(vulkLogicalDevice, nullptr);
	}


	VkDevice& VulkanLogicalDevice::GetVkLogicalDevice()
	{
		return vulkLogicalDevice;
	}


	VkQueue& VulkanLogicalDevice::GetVkQueue(const VulkanQueueFamilies::IDs idx_)
	{
		return vulkQueues[idx_];
	}


	void VulkanLogicalDevice::WaitFor()
	{
		vkDeviceWaitIdle(vulkLogicalDevice);

		for (u8 i = 0; i < VulkanQueueFamilies::Num; ++i)
		{
			vkQueueWaitIdle(GetVkQueue((VulkanQueueFamilies::IDs)i));
		}
	}


	bool VulkanLogicalDevice::CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char* const* requiredExtensions_) const
	{
		// Get max number of extensions
		uint32_t nAllExtensions = 0;
		VULK_ASSERT_SUCCESS(vkEnumerateDeviceExtensionProperties, vulkanPhysicalDevice.GetVkPhysicalDevice(), nullptr, &nAllExtensions, nullptr);

		// Allocate mem to store and get full list
		Utility::MemoryBlock allExtensionBlk = stackAllocator_.Allocate<VkExtensionProperties>(nAllExtensions);
		VkExtensionProperties* allExtensions = (VkExtensionProperties*)allExtensionBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumerateDeviceExtensionProperties, vulkanPhysicalDevice.GetVkPhysicalDevice(), nullptr, &nAllExtensions, allExtensions);

		// Allocate mem to check against required extensions
		Utility::MemoryBlock extensionsFoundBlk = stackAllocator_.Allocate<u8>(nRequiredExtensions_);
		u8* extensionsFound = (u8*)extensionsFoundBlk.ptr;

		// Mark all extensions as not existing
		memset(extensionsFound, 0, extensionsFoundBlk.size);
		bool haveAllExtensions = true;

		for (usize i = 0; i < nRequiredExtensions_; ++i)
		{
			for (usize j = 0; j < nAllExtensions; ++j)
			{
				if (!strcmp(requiredExtensions_[i], allExtensions[j].extensionName))
				{
					// Mark as found in list
					extensionsFound[i] = 1;
					break;
				}
			}

			if (!extensionsFound[i])
			{
				Utility::Log(Utility::Error, "Missing %s extension for new VkInstance!", requiredExtensions_[i]);
				haveAllExtensions = false;
			}
		}

		// Free mem
		stackAllocator_.Free(extensionsFoundBlk);
		stackAllocator_.Free(allExtensionBlk);

		return haveAllExtensions;
	}


	bool VulkanLogicalDevice::CheckRequiredValidationLayers(Utility::StackAllocator& stackAllocator_, const usize nRequiredLayers_, const char* const* requiredLayers_) const
	{
		// Get max number of layers
		uint32_t nAllLayers = 0;
		VULK_ASSERT_SUCCESS(vkEnumerateDeviceLayerProperties, vulkanPhysicalDevice.GetVkPhysicalDevice(), &nAllLayers, nullptr);

		// Allocate mem to store and get full list
		Utility::MemoryBlock allLayersBlk = stackAllocator_.Allocate<VkLayerProperties>(nAllLayers);
		VkLayerProperties* allLayers = (VkLayerProperties*)allLayersBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumerateDeviceLayerProperties, vulkanPhysicalDevice.GetVkPhysicalDevice(), &nAllLayers, allLayers);

		// Allocate mem to check against required layers
		Utility::MemoryBlock layersFoundBlk = stackAllocator_.Allocate<u8>(nRequiredLayers_);
		u8* layersFound = (u8*)layersFoundBlk.ptr;

		// Mark all layers as not existing
		memset(layersFound, 0, layersFoundBlk.size);
		bool haveAllLayers = true;

		for (usize i = 0; i < nRequiredLayers_; ++i)
		{
			for (usize j = 0; j < nAllLayers; ++j)
			{
				if (!strcmp(requiredLayers_[i], allLayers[j].layerName))
				{
					// Mark as found in list
					layersFound[i] = 1;
					break;
				}
			}

			if (!layersFound[i])
			{
				Utility::Log(Utility::Error, "Missing %s layers for new VkDevice!", requiredLayers_[i]);
				haveAllLayers = false;
			}
		}

		// Free mem
		stackAllocator_.Free(layersFoundBlk);
		stackAllocator_.Free(allLayersBlk);

		return haveAllLayers;
	}
}
