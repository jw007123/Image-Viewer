#include "Rendering/Vulkan/Impl/VulkanInstance.h"

namespace Rendering
{
	VulkanInstance::VulkanInstance(Utility::StackAllocator& stackAllocator_)
	{
		VkApplicationInfo vulkAppInfo  = {};
		vulkAppInfo.sType			   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vulkAppInfo.pApplicationName   = "Image Viewer";
		vulkAppInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
		vulkAppInfo.pEngineName		   = "Ian Engine";
		vulkAppInfo.engineVersion	   = VK_MAKE_API_VERSION(1, 0, 0, 0);
		vulkAppInfo.apiVersion		   = VK_API_VERSION_1_3;

		// Use glfw to obtain extensions required for window creation
		uint32_t glfwExtensionCnt	   = 0;
		const char** glfwExtensionsPtr = glfwGetRequiredInstanceExtensions(&glfwExtensionCnt);
		if (!CheckRequiredExtensions(stackAllocator_, glfwExtensionCnt, glfwExtensionsPtr))
		{
			assert(0);
		}

		VkInstanceCreateInfo vulkInstanceCreateInfo    = {};
		vulkInstanceCreateInfo.sType				   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vulkInstanceCreateInfo.pApplicationInfo        = &vulkAppInfo;
		vulkInstanceCreateInfo.enabledExtensionCount   = glfwExtensionCnt;
		vulkInstanceCreateInfo.ppEnabledExtensionNames = glfwExtensionsPtr;

#ifdef NDEBUG
		// Disable validation layers in release
		vulkInstanceCreateInfo.enabledLayerCount   = 0;
		vulkInstanceCreateInfo.ppEnabledLayerNames = nullptr;
#else
		// Check required layers are available
		if (!CheckRequiredValidationLayers(stackAllocator_, Consts::nRequiredLayers, Consts::debugValidationLayer))
		{
			assert(0);
		}

		vulkInstanceCreateInfo.enabledLayerCount   = Consts::nRequiredLayers;
		vulkInstanceCreateInfo.ppEnabledLayerNames = Consts::debugValidationLayer;
#endif

		VULK_ASSERT_SUCCESS(vkCreateInstance, &vulkInstanceCreateInfo, nullptr, &vulkInstance);
	}


	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(vulkInstance, nullptr);
	}


	VkInstance& VulkanInstance::GetVkInstance()
	{
		return vulkInstance;
	}


	bool VulkanInstance::CheckRequiredValidationLayers(Utility::StackAllocator& stackAllocator_, const usize nRequiredLayers_, const char* const* requiredLayers_) const
	{
		// Get max number of layers
		uint32_t nAllLayers = 0;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceLayerProperties, &nAllLayers, nullptr);

		// Allocate mem to store and get full list
		Utility::MemoryBlock allLayersBlk = stackAllocator_.Allocate<VkLayerProperties>(nAllLayers);
		VkLayerProperties* allLayers      = (VkLayerProperties*)allLayersBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceLayerProperties, &nAllLayers, allLayers);

		// Allocate mem to check against required layers
		Utility::MemoryBlock layersFoundBlk = stackAllocator_.Allocate<u8>(nRequiredLayers_);
		u8* layersFound						= (u8*)layersFoundBlk.ptr;

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
				Utility::Log(Utility::Error, "Missing %s layers for new VkInstance!", requiredLayers_[i]);
				haveAllLayers = false;
			}
		}

		// Free mem
		stackAllocator_.Free(layersFoundBlk);
		stackAllocator_.Free(allLayersBlk);

		return haveAllLayers;
	}


	bool VulkanInstance::CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char* const* requiredExtensions_) const
	{
		// Get max number of extensions
		uint32_t nAllExtensions = 0;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceExtensionProperties, nullptr, &nAllExtensions, nullptr);

		// Allocate mem to store and get full list
		Utility::MemoryBlock allExtensionBlk = stackAllocator_.Allocate<VkExtensionProperties>(nAllExtensions);
		VkExtensionProperties* allExtensions = (VkExtensionProperties*)allExtensionBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceExtensionProperties, nullptr, &nAllExtensions, allExtensions);

		// Allocate mem to check against required extensions
		Utility::MemoryBlock extensionsFoundBlk = stackAllocator_.Allocate<u8>(nRequiredExtensions_);
		u8* extensionsFound						= (u8*)extensionsFoundBlk.ptr;

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
}
