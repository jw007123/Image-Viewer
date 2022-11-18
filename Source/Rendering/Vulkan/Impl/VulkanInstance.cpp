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
#if _DEBUG
		vulkInstanceCreateInfo.enabledLayerCount	   = 0;
		vulkInstanceCreateInfo.ppEnabledLayerNames	   = nullptr;
#else
		// Disable validation layers in release
		vulkInstanceCreateInfo.enabledLayerCount	   = 0;
		vulkInstanceCreateInfo.ppEnabledLayerNames	   = nullptr;
#endif

		VULK_ASSERT_SUCCESS(vkCreateInstance, &vulkInstanceCreateInfo, nullptr, &vulkInstance);
	}


	VulkanInstance::~VulkanInstance()
	{
		vkDestroyInstance(vulkInstance, nullptr);
	}


	bool VulkanInstance::CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nRequiredExtensions_, const char** requiredExtensions_) const
	{
		// Get max number of extensions
		uint32_t nAllExtensions = 0;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceExtensionProperties, nullptr, &nAllExtensions, nullptr);

		// Allocate mem to store and get full list
		Utility::MemoryBlock allExtensionBlk = stackAllocator_.Allocate(nAllExtensions * Consts::maxNameLength);
		VkExtensionProperties* allExtensions = (VkExtensionProperties*)allExtensionBlk.ptr;
		VULK_ASSERT_SUCCESS(vkEnumerateInstanceExtensionProperties, nullptr, &nAllExtensions, allExtensions);

		// Allocate mem to check against extensions_
		Utility::MemoryBlock extensionsFoundBlk = stackAllocator_.Allocate(nRequiredExtensions_);
		u8* extensionsFound						= (u8*)extensionsFoundBlk.ptr;

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
		}

		// Check for missing extensions
		bool haveAllExtensions = true;
		for (usize i = 0; i < nRequiredExtensions_; ++i)
		{
			if (extensionsFound[i] == 1)
			{
				continue;
			}

			Utility::Log(Utility::Error, "Missing %s extension for new VkInstance!", requiredExtensions_[i]);
			haveAllExtensions = false;
		}

		// Free mem
		stackAllocator_.Free(extensionsFoundBlk);
		stackAllocator_.Free(allExtensionBlk);

		return haveAllExtensions;
	}
}
