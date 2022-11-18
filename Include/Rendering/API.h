#pragma once

//#if defined(RENDERER_API_OPENGL)
	#include "Rendering/OpenGL/Backend.h"
	#include "Rendering/OpenGL/Framebuffer.h"
	#include "Rendering/OpenGL/Program.h"
	#include "Rendering/OpenGL/Renderer.h"
	#include "Rendering/OpenGL/Shader.h"
	#include "Rendering/OpenGL/Texture.h"
//#elif defined(RENDERER_API_VULKAN)
	#define GLFW_INCLUDE_VULKAN
	#define VK_USE_PLATFORM_WIN32_KHR

	#include "Rendering/Vulkan/Impl/VulkanUtility.h"
	#include "Rendering/Vulkan/Impl/VulkanInstance.h"

	#include "Rendering/Vulkan/Backend.h"
//#else
//	#error "Unknown rendering API!"
//#endif
