#pragma once

//#if defined(RENDERER_API_OPENGL)
	#include "Rendering/OpenGL/Backend.h"
	#include "Rendering/OpenGL/Framebuffer.h"
	#include "Rendering/OpenGL/Program.h"
	#include "Rendering/OpenGL/Renderer.h"
	#include "Rendering/OpenGL/Shader.h"
	#include "Rendering/OpenGL/Texture.h"
//#elif defined(RENDERER_API_VULKAN)
	#include "Rendering/Vulkan/Impl/VulkanUtility.h"
	#include "Rendering/Vulkan/Impl/VulkanInstance.h"
	#include "Rendering/Vulkan/Impl/VulkanImGui.h"
	#include "Rendering/Vulkan/Impl/VulkanGLFW.h"
	#include "Rendering/Vulkan/Impl/VulkanPhysicalDevice.h"
	#include "Rendering/Vulkan/Impl/VulkanLogicalDevice.h"
	#include "Rendering/Vulkan/Impl/VulkanQueueFamilies.h"
	#include "Rendering/Vulkan/Impl/VulkanVma.h"
	#include "Rendering/Vulkan/Impl/VulkanSurface.h"
	#include "Rendering/Vulkan/Impl/VulkanSwapChain.h"
	#include "Rendering/Vulkan/Impl/VulkanShader.h"
	#include "Rendering/Vulkan/Impl/VulkanFramebuffer.h"
	#include "Rendering/Vulkan/Impl/VulkanCommandPool.h"
	#include "Rendering/Vulkan/Impl/VulkanMeshData.h"
	#include "Rendering/Vulkan/Impl/VulkanUBO.h"

	#include "Rendering/Vulkan/Backend.h"
//#else
//	#error "Unknown rendering API!"
//#endif
