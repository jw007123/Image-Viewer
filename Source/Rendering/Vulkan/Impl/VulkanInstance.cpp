#include "Rendering/Vulkan/Impl/VulkanInstance.h"

namespace Rendering
{
	VulkanInstance::VulkanInstance()
	{
		initd = false;
	}


	void VulkanInstance::Init(Utility::StackAllocator& stackAllocator_)
	{
		VkApplicationInfo vulkAppInfo  = {};
		vulkAppInfo.sType			   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vulkAppInfo.pApplicationName   = "Image Viewer";
		vulkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		vulkAppInfo.pEngineName		   = "Ian Engine";
		vulkAppInfo.engineVersion	   = VK_MAKE_VERSION(1, 0, 0);
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
		initd = true;
	}


	VulkanInstance::~VulkanInstance()
	{
		if (initd)
		{
			vkDestroyInstance(vulkInstance, nullptr);
		}
	}


	bool VulkanInstance::CheckRequiredExtensions(Utility::StackAllocator& stackAllocator_, const usize nExtensions_, const char** extensions_) const
	{
		uint32_t vulkExtensionCnt = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &vulkExtensionCnt, nullptr);

		return true;
	}
}
