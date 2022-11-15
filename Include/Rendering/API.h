#pragma once

#if defined(RENDERER_API_OPENGL)
	#include "Rendering/OpenGL/Backend.h"
	#include "Rendering/OpenGL/Framebuffer.h"
	#include "Rendering/OpenGL/Program.h"
	#include "Rendering/OpenGL/Renderer.h"
	#include "Rendering/OpenGL/Shader.h"
	#include "Rendering/OpenGL/Texture.h"
#elif defined(RENDERER_API_VULKAN)
	
#else
	#error "Unknown rendering API!"
#endif
